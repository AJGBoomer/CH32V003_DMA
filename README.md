# CH32V003_DMA
Using DMA in a CH32V003 to drive GRB leds without bitbang.
/* DMA RGB LED Example
 * Author			  : A. J. Griggs
 * Date				  : 2024/01/09
 * Description        : Main program body.
 *	Using the SPI example from WCH, add GRB (WS2812) encoding to demonstrate using DMA
 *	with SPI rather than bit-banging.
 *	You Must complete building half of the DMA half buffer (top or bottom) before DMA to transfer it.
 *	For this example, filling the buffer took 32.5% of the total processing time compiled in 'fast' optimization.
 *	87% with No optimization.
 *	43% with Optimize (O)
 *	17.5% with Optimize (O) removing the (not needed) amplitude multiply in setLed()
 *	14% as above in optimize fast
 *	Timings are from scope traces of the DMA service loop and don't include interrupt processing (which should be minimal)
 */
