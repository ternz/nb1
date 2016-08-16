package ct

import (
	"bytes"
	"time"
	"fmt"
	"math/rand"
	"encoding/binary"
	"errors"
)

func init() {
	rand.Seed(time.Now().UnixNano())
}

const (
	ERROR = 0
	READ = 1
	WRITE = 2
	READ_WRITE = 3
)

type Record struct {
	TimeBegin	int64
	TimeEnd		int64
	DataSize	uint32
	Type		int	//1 for read, 2 for write
	Err			error
}

func (r *Record) DataSizeStr() string {
	l := 0
	size := r.DataSize
	d := size / 1024
	for d != 0 {
		l ++
		size = d
		d = size / 1024
	}
	var c string
	if l == 0 {
		c = "B"
	} else if l == 1 {
		c = "KB"
	} else if l == 2 {
		c = "MB"
	} else {
		c = "GB"
	}
	return fmt.Sprintf("%d%s", size, c)
	
}

func (r *Record) DurationTime() int64 {
	return r.TimeEnd - r.TimeBegin
}

func (r *Record) DurationTimeStr() string {
	t := float64(r.DurationTime()) / 1000000
	return fmt.Sprintf("%.2fms", t)
}

func (r *Record) TypeStr() string {
	switch(r.Type) {
		case ERROR:
			return "ERROR"
		case READ:
			return "READ"
		case WRITE:
			return "WRITE"
		default:
			return "NONE"
	}
}

var RecordChan =  make(chan *Record, 1024)

type Statistics struct {
	StartTime		int64
	StopTime		int64
	TotalDataSize	uint64
	IOTimes			uint32
	AvgDur			float64
}

func (s *Statistics) Add(dataSize uint64, dur int64) {
	totalDur := s.AvgDur * float64(s.IOTimes) + float64(dur)
	s.IOTimes += 1
	s.AvgDur = totalDur / float64(s.IOTimes)
	s.TotalDataSize += dataSize
}

func (s *Statistics) AvgDurS() float64 {
	return s.AvgDur / 1000000000
}

func (s *Statistics) TotalDur() int64 {
	return s.StopTime - s.StartTime
}

func (s *Statistics) TotalDurS() float64 {
	return float64(s.TotalDur()) / 1000000000
}

func (s *Statistics) IOPS() float64 {
	return float64(s.IOTimes) / float64(s.TotalDurS())
}

func (s *Statistics) AvgBW() float64 {
	return float64(s.TotalDataSize) / float64(s.TotalDurS() * 1024 * 1024)
}

var statis = &Statistics{}

func CreateChunk(size uint32) []byte {
	b := make([]byte, int(size))
	for i:=0; i<int(size); i++ {
		b[i] = 0x41
	}
	return b
}

func CreateChunk1(size int) []byte {
	rd := rand.New(rand.NewSource(time.Now().Unix()))
	buf := make([]byte, size)
	buf_w := bytes.NewBuffer(buf)
	var n int = 0
	var err error
	var x int
	for n < size {
		x= rd.Int()
		binary.Write(buf_w, binary.LittleEndian, &x)
		if err != nil {
			fmt.Println(err)
			return nil
		}
		n += 4
	}
	return buf
} 

//new
func Read(file string) {
	r := &Record{}
	r.Type = READ
	
	r.TimeBegin = time.Now().UnixNano()
	b, _ := client.GetObject(bucket, file)
	r.TimeEnd = time.Now().UnixNano()
	
	r.DataSize = uint32(len(b))
	RecordChan <- r
}

func DoReadTest(c *Config) {
	for i:=0; i<c.LoopNum; i++ {
		go ReadLoop(c.ReadFile)
	}
}

func ReadLoop(file string) {
	wg.Add(1)
	defer wg.Done()
	for run {
		Read(file)
	}
}

func Write(b []byte, bucket, obj string, check_md5 bool) {
	r := &Record{}
	r.Type = WRITE
	r.DataSize = uint32(len(b))
	str_b := string(b)
	
	r.TimeBegin = time.Now().UnixNano()
	client.PutObject(bucket, obj, &str_b, check_md5)
	r.TimeEnd = time.Now().UnixNano()
	RecordChan <- r
}

func DoWriteTest(c *Config) {
	b := CreateChunk(c.DataSize)
	for i:=0; i<c.LoopNum; i++ {
		
		go WriteLoop(b, c)
	}
}

func WriteLoop(b []byte, c *Config) {
	wg.Add(1)
	defer wg.Done()
	for run {
		Write(b, c.Bucket, c.WriteFile, c.Checkmd5)
	}
}

func DoReadWriteTest(c *Config) {
	wb := CreateChunk(c.DataSize)
	for i:=0; i<c.LoopNum; i++ {
		go ReadWriteLoop(wb, c)
	}
}

//初始化发牌
func InitCards(cards []int8, rf, wf int) {
	length := len(cards)
	if rf > length || wf > length {
		rf = length / 2
		wf = length - rf
	}
	for i:=0; i<length; i++ {
		if i < rf {
			cards[i] = READ
		} else {
			cards[i] = WRITE
		}
	}
	var tmp, n int
	for i:=0; i<length; i++ {
		n = rand.Intn(length)
		
		//swap
		tmp = int(cards[i])
		cards[i] = cards[n]
		cards[n] = int8(tmp)
	}
}

func ReadWriteLoop(wb []byte, c *Config) {
	wrl := c.WriteFactor + c.ReadFactor
	cards := make([]int8, wrl)
	cards_remain := 0
	
	wg.Add(1)
	defer wg.Done()
	for run {
		if(cards_remain == 0) {
			InitCards(cards, c.ReadFactor, c.WriteFactor)
			cards_remain = wrl
		}
		cards_remain -= 1
		if cards[cards_remain] == READ {
			Read(c.ReadFile)
		} else {
			Write(wb, c.Bucket, c.WriteFile, c.Checkmd5)
		}
	}
}

func HandleRecord(r *Record) {
	statis.Add(uint64(r.DataSize), r.DurationTime())
	fmt.Printf("%s data size: %s duration: %s\n", r.TypeStr(), r.DataSizeStr(), r.DurationTimeStr())
}

func StatisticsLoop() {
	//TODO
	statis.StartTime = time.Now().UnixNano()
	timer_dur := time.Second
	timer := time.NewTimer(timer_dur)
	wg.Add(1)
	defer wg.Done()
	for run {
		select {
			case r := <- RecordChan: {
				HandleRecord(r)
			}
			case <-timer.C: {
				timer.Reset(timer_dur)
			}
		}
	}
	
}

func LastStatistics() {
	select {
		case r := <- RecordChan: {
			HandleRecord(r)
		}
		default:
	}
	statis.StopTime = time.Now().UnixNano()
	
	fmt.Printf("test duration %.2fs IOPS %.2f avg delay %.4fs bw %.4fmb/s\n", 
		statis.TotalDurS(),
		statis.IOPS(),
		statis.AvgDurS(),
		statis.AvgBW())
}

func RunTest(c *Config) error {
	switch(c.Type) {
		case READ: {
			DoReadTest(c)
		}
		case WRITE: {
			DoWriteTest(c)
		}
		case READ_WRITE: {
			DoReadWriteTest(c)
		}
		default: 
			return errors.New("unknown test type")
	}
	go StatisticsLoop()
	return nil
}