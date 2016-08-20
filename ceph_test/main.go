package main

import (
	"strings"
	"io/ioutil"
	"os"
	"flag"
	"fmt"
	"time"
	"ceph_test/ct"
)

func fileList(path string) []string {
	b, err := ioutil.ReadFile(path)
	if err != nil {
		fmt.Errorf("read file %s error: %s", path, err.Error())
		os.Exit(1)
	}
	return strings.Fields(string(b))
}

func parseArg(c *ct.Config) {
	ds_ptr := flag.Int("c", 200, "data size for write")
	cm_ptr := flag.Bool("check-md5", false, "is check md5")
	th_ptr := flag.Int("p", 4, "number of threads")
	ts_ptr := flag.Int("t", 60, "test running duration (s)")
	type_ptr := flag.String("type", "w", "type of testing (\"r\", \"w\", or \"rw\")")
	r_ptr := flag.Int("r", 5, "read ratio in rw mode")
	w_ptr := flag.Int("w", 5, "write ratio in rw mode")
	bk_ptr := flag.String("bucket", "ct_test_bucket_1", "bucket for test")
	wf_ptr := flag.String("wfile-prefix", "ct_file_w", "file name prefix for write")
	rf_ptr := flag.String("rfile", "ct_file_r.txt", "file name for read")
	ep_ptr := flag.String("u", "", "enpoint of test server")
	ak_ptr := flag.String("ak", "", "access key")
	sk_ptr := flag.String("sk", "", "secret key")
	rl_ptr := flag.String("rfile-list", "", "file save files for reading")
	
	flag.Parse()
	
	c.DataSize = uint32(*ds_ptr * 1024)
	c.Checkmd5 = *cm_ptr
	c.LoopNum = *th_ptr
	c.Time = *ts_ptr
	switch(*type_ptr) {
		case "r":
			c.Type =  ct.READ
		case "w":
			c.Type =  ct.WRITE
		case "rw":
			c.Type = ct.READ_WRITE
		default:
			fmt.Errorf("arg error\n")
			os.Exit(1)
	}
	c.ReadFactor = *r_ptr
	c.WriteFactor = *w_ptr
	c.Bucket = *bk_ptr
	c.WriteFile = *wf_ptr
	//c.ReadFile = *rf_ptr
	c.Endpoint = *ep_ptr
	c.AccessKey = *ak_ptr
	c.SecretKey = *sk_ptr
	if *rl_ptr != "" {
		c.ReadFileList = fileList(*rl_ptr)
	} else {
		c.ReadFileList = append(c.ReadFileList, *rf_ptr)
	}
}

func main() {
	fmt.Println("start test")
	cfg := &ct.Config{}
	
	parseArg(cfg)
	
	fmt.Printf("%+v\n",cfg)
	
	/*cfg.DataSize = 200 * 1024
	cfg.Checkmd5 = false
	cfg.LoopNum = 4
	cfg.Time = 10*60
	cfg.Type = ct.READ_WRITE
	cfg.Bucket = "ct_test_bucket_1"
	cfg.WriteFile = "ct_file2.txt"
	cfg.ReadFile = "ct_file1.txt"
	cfg.WriteFactor = 5
	cfg.ReadFactor = 5*/
	
	ct.InitRgwClient(cfg)
	
	ctl := ct.NewController();
	ctl.StartTimer(time.Second * time.Duration(cfg.Time))
	
	err := ct.RunTest(cfg)
	if err != nil {
		fmt.Println(err)
		return
	}
	
	ctl.Wait()
	
	ct.LastStatistics()
	fmt.Println("stop test")
}
