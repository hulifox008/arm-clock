/* this takes the image strip saved by GIMP, convert it to memory image that is easy to use in the arm-clock code.
 * Converted image has the following format:
 *
 *   Each character is represented by black/white 40x56 pixels. Each line is store in 5 bytes (5 x 8bits = 40bits). And there are 56 lines. Then there's totl 5*56=280 bytes for each characters. All characters are stored in continus memory.
 */

int main()
{
}
