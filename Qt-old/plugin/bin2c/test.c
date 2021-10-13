#include	<stdio.h>

extern char		bin2c_data[];
extern const int	bin2c_data_len;

int main(void)
{
    int	i;

    for (i = 0; i < bin2c_data_len; ++i)
      putchar(bin2c_data[i]);
    
    return 0;
}
