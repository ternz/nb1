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
	
	ten_p_6 = 1000000
	ten_p_9 = 1000000000
)

var file_no uint64 = 0
var read_n int = 0

type Record struct {
	TimeBegin	int64
	TimeEnd		int64
	DataSize	uint32
	Type		int	//1 for read, 2 for write
	Err			error
	Obj			string
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
	t := float64(r.DurationTime()) / ten_p_6
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
	return s.AvgDur / ten_p_9
}

func (s *Statistics) TotalDur() int64 {
	return s.StopTime - s.StartTime
}

func (s *Statistics) TotalDurS() float64 {
	return float64(s.TotalDur()) / ten_p_9
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
func Read(bucket, file string) {
	r := &Record{}
	r.Type = READ
	r.Obj = file
	
	r.TimeBegin = time.Now().UnixNano()
	b, err := client.GetObject(bucket, file)
	r.TimeEnd = time.Now().UnixNano()
	
	r.Err = err
	
	r.DataSize = uint32(len(b))
	RecordChan <- r
}

func DoReadTest(c *Config) {
	for i:=0; i<c.LoopNum; i++ {
		go ReadLoop(c.Bucket, c.ReadFileList)
	}
}

func ReadLoop(bucket string, files []string) {
	
	wg.Add(1)
	defer wg.Done()
	for run {
		a := read_n
		if a >= len(files) {
			a = 0
			read_n = 0
		} else {
			read_n += 1
		}
		Read(bucket, files[a])
	}
}

func Write(b []byte, bucket, obj string, check_md5 bool) {
	r := &Record{}
	r.Type = WRITE
	r.Obj = obj
	r.DataSize = uint32(len(b))
	str_b := string(b)
	
	r.TimeBegin = time.Now().UnixNano()
	err := client.PutObject(bucket, obj, &str_b, check_md5)
	r.TimeEnd = time.Now().UnixNano()
	r.Err = err
	RecordChan <- r
}

func DoWriteTest(c *Config) {
	b := CreateChunk(c.DataSize)
	for i:=0; i<c.LoopNum; i++ {
		
		go WriteLoop(b, c)
	}
}

func WriteLoop(b []byte, c *Config) {
	
	var file_name string
	
	wg.Add(1)
	defer wg.Done()
	for run {
		file_no++
		file_name = fmt.Sprintf("%s%d.txt", c.WriteFile, file_no)
		Write(b, c.Bucket, file_name, c.Checkmd5)
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
	
	
	
	var file_name string
	
	wg.Add(1)
	defer wg.Done()
	for run {
		if(cards_remain == 0) {
			InitCards(cards, c.ReadFactor, c.WriteFactor)
			cards_remain = wrl
		}
		cards_remain -= 1
		if cards[cards_remain] == READ {
			a := read_n
			if a >= len(c.ReadFileList) {
				a = 0
				read_n = 0
			} else {
				read_n += 1
			}
			Read(c.Bucket, c.ReadFileList[a])

		} else {
			file_no++
			file_name = fmt.Sprintf("%s%d.txt", c.WriteFile, file_no)
			Write(wb, c.Bucket, file_name, c.Checkmd5)
		}
	}
}

func HandleRecord(r *Record) {
	statis.Add(uint64(r.DataSize), r.DurationTime())
	time_str := time.Unix(r.TimeEnd / ten_p_9, r.TimeEnd % ten_p_9).Format("2006-01-02 15:04:05")
	if r.Err != nil {
		fmt.Printf("[%s] %s size:%s duration:%s file:%s ERROR:%s  \n",
			time_str, r.TypeStr(), r.DataSizeStr(), r.DurationTimeStr(), r.Obj, r.Err.Error())
	} else {
		fmt.Printf("[%s] %s size:%s duration:%s file:%s\n",
			time_str, r.TypeStr(), r.DataSizeStr(), r.DurationTimeStr(), r.Obj)
	}
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