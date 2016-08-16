package ct

import (
	"time"
	"os"
	"os/signal"
	"syscall"
	"sync"
)

var wg sync.WaitGroup 
var run = true

type Controller struct {
	ChanSig chan os.Signal
	Timer *time.Timer
}

func NewController() *Controller {
	c := &Controller{}
	c.Init()
	return c
}

func (c *Controller) Init() {
	c.ChanSig = make(chan os.Signal)
	signal.Notify(c.ChanSig, syscall.SIGINT)
	signal.Notify(c.ChanSig, syscall.SIGTERM)
	//signal.Ignore(syscall.SIGHUP)
}

func (c *Controller) StartTimer(t time.Duration) {
	c.Timer = time.NewTimer(t)
}

func (c *Controller) Wait() {
	select {
		case <-c.Timer.C:
		case <-c.ChanSig:
	}
	run = false
	wg.Wait()
}