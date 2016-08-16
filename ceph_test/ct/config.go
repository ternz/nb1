package ct

import (
	"fmt"
	"common/rgw_client/rgw_client"
	"common/rgw_client/sign"
)

var endpoint = "http://172.20.1.10:7480"	
var auth = sign.Auth{"IGAJ9M9V8MA5UE503HZE","CwcGOZokDqdPcBhxkWxmvIyJLA79DP3ycCxHkGhO"}
var bucket = "ct_test_bucket_1"
var obj_file = "ct_file1.txt"

var client *rgw_client.RgwClient



func InitRgwClient(c *Config) {
	client = &rgw_client.RgwClient{}
	if c.AccessKey == "" || c.SecretKey == "" {
		client.Auth = auth
	} else {
		client.Auth = sign.Auth{c.AccessKey, c.SecretKey}
	}
	if c.Endpoint == "" {
		client.Endpoint = endpoint
	}
	
	err := client.CreateBucket(c.Bucket)
	
	if (err != nil) {
		fmt.Printf("create bucket %s error: %s\n", c.Bucket, err.Error())
	} else {
		fmt.Printf("create bucket %s success\n", c.Bucket)
	}
}

type Config struct {
	DataSize	uint32
	Checkmd5	bool
	WriteFactor int
	ReadFactor	int
	LoopNum		int
	Time		int  //s
	Type		int
	
	Endpoint	string
	AccessKey	string
	SecretKey	string
	
	Bucket		string
	WriteFile	string
	ReadFile	string
}