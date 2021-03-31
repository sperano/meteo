package cmd

import (
	"fmt"
	"image/color"
	"log"
	"os"

	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"golang.org/x/image/bmp"
)

var (
	loglevel string
	cfgFile  string
	compress bool
	rootCmd  = &cobra.Command{
		Use:   "bitmap6809 <source bitmap> <asm file to write>",
		Short: "Bitmap to Apple II C Code",
		Long: `Bitmap to Apple II C Code

(c) 2021 Éric Spérano`,
		Args: cobra.ExactArgs(2),
		Run: func(cmd *cobra.Command, args []string) {
			file, err := os.Open(args[0])
			defer file.Close()
			if err != nil {
				log.Fatal(err)
			}
			img, err := bmp.Decode(file)
			if err != nil {
				log.Fatal(err)
			}
			outfile, err := os.Create(args[1])
			defer outfile.Close()
			if err != nil {
				log.Fatal(err)
			}
			bounds := img.Bounds()
			fmt.Printf("bounds: %+v\n", bounds)
			colorset := make(map[color.Color]byte)
			var currcol byte = 0
			for y := 0; y < bounds.Dy(); y++ {
				for x := 0; x < bounds.Dx(); x++ {
					col := img.At(x, y)
					if _, ok := colorset[col]; !ok {
						colorset[img.At(x, y)] = currcol
						//currcol++ TODO TODO TODO TODO
						currcol += 0x0d
					}
				}
			}
			fmt.Printf("%d color(s)\n", len(colorset))
			fmt.Fprintf(outfile, "unsigned char Bitmap[20][40] = {\n")
			for y := 0; y < 20; y++ {
				fmt.Fprintf(outfile, "   {")
				for x := 0; x < 40; x++ {
					if x > 0 {
						fmt.Fprintf(outfile, ", ")
					}
					pixel1 := colorset[img.At(x, y*2)]
					pixel2 := colorset[img.At(x, (y*2)+1)]
					fmt.Fprintf(outfile, "%d", pixel1|(pixel2<<4))
				}
				fmt.Fprintf(outfile, "},\n")
			}
			fmt.Fprintf(outfile, "};\n")
		},
	}
)

func init() {
	cobra.OnInitialize(initCobra)
	rootCmd.PersistentFlags().BoolVarP(&compress, "compress", "c", false, "Compressed mode")
}

func initCobra() {
	if cfgFile != "" {
		// Use config file from the flag.
		viper.SetConfigFile(cfgFile)
	} else {
		// Search config in home directory with name ".cobra" (without extension).
		viper.AddConfigPath(os.Getenv("HOME"))
		viper.SetConfigName(".cobra")
	}

	viper.AutomaticEnv()

	if err := viper.ReadInConfig(); err == nil {
		fmt.Println("Using config file:", viper.ConfigFileUsed())
	}
}

/*
func printVersionString(nestor c.Nestor) {
	shell, err := nestor.GetCurrentShell()
	if err != nil {
		panic(err)
	}
	txt := fmt.Sprintf("%s version %s - %s - %s",
		strings.Title(c.AppName), c.Version,
		strings.ToLower(nestor.Runtime().GetCurrentOSID().String()),
		strings.ToLower(shell.ShellID.String()))
	fmt.Printf("%s\n", txt)
}

*/
// Execute executes the root command.
func Execute() error {
	return rootCmd.Execute()
}
