package cmd

import (
	"fmt"
	"log"
	"os"
	"strconv"
	"strings"

	"github.com/spf13/cobra"
	"golang.org/x/image/bmp"
)

var (
	rootCmd = &cobra.Command{
		Use:   "bitmap6809 <source bitmap> <asm file to write>",
		Short: "Bitmap to Apple II C Code",
		Long: `Bitmap to Apple II C Code

Use it like this:
cat image.bmp | bitmap2c 1=13 4=12 >code.h

on the command line, pass color mappings:

1=13 means that the color #1 will be color #13 in the C code

(c) 2021 Éric Spérano`,
		//Args: cobra.ExactArgs(1),
		Run: func(cmd *cobra.Command, args []string) {
			colorMaps := make(map[string]int)
			colorMaps["0000000000000000"] = 0
			colorMaps["000000000000ffff"] = 0
			colorMaps["ffffffffffffffff"] = 15
			for _, arg := range args {
				tokens := strings.Split(arg, "=")
				if len(tokens) != 2 {
					fmt.Fprintf(os.Stderr, "Invalid argument: %v\n", tokens)
					os.Exit(1)
				}
				value, err := strconv.Atoi(tokens[1])
				if err != nil {
					fmt.Fprintf(os.Stderr, "%s\n", err)
					os.Exit(1)
				}
				colorMaps[tokens[0]] = value
			}
			// load image
			img, err := bmp.Decode(os.Stdin)
			if err != nil {
				log.Fatal(err)
			}
			getPixel := func(x, y int) int {
				color := img.At(x, y)
				r, g, b, a := color.RGBA()
				rgbaStr := fmt.Sprintf("%04x%04x%04x%04x", r, g, b, a)
				value, ok := colorMaps[rgbaStr]
				if !ok {
					fmt.Fprintf(os.Stderr, "No color mapping for %s\n", rgbaStr)
					os.Exit(1)
				}
				return value
			}
			bounds := img.Bounds()
			fmt.Fprintf(os.Stderr, "bounds: %+v\n", bounds)
			fmt.Printf("unsigned char Bitmap[20][40] = {\n")
			for y := 0; y < 20; y++ {
				fmt.Printf("   {")
				for x := 0; x < bounds.Dx(); x++ {
					if x > 0 {
						fmt.Printf(", ")
					}
					pixel1 := getPixel(x, y*2)
					pixel2 := getPixel(x, (y*2)+1)
					fmt.Printf("%d", pixel1|(pixel2<<4))
				}
				fmt.Printf("},\n")
			}
			fmt.Printf("};\n")
		},
	}
)

func init() {
	//cobra.OnInitialize(initCobra)
	//rootCmd.PersistentFlags().BoolVarP(&compress, "compress", "c", false, "Compressed mode")
}

// Execute executes the root command.
func Execute() error {
	return rootCmd.Execute()
}
