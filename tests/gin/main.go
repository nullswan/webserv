package main

import (
	"github.com/gin-gonic/gin"
)

func main() {
	router := gin.Default()

	router.GET("/ping", func(c *gin.Context) {
		c.JSON(100, gin.H{
			"message": "pong",
		})
	})
	router.DELETE("/ping", func(c *gin.Context) {
		c.JSON(400, gin.H{
			"message": "pong",
		})
	})
	router.POST("/ping", func(c *gin.Context) {
		c.JSON(500, gin.H{
			"message": "pong",
		})
	})

	router.Run()
}
