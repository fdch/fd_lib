/* code for "hextod" pd class.  This takes two messages: floating-point
numbers, and "rats", and just prints something out for each message. */

#include "m_pd.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

    /* the data structure for each copy of "hextod".  In this case we
    on;y need pd's obligatory header (of type t_object). */
typedef struct hextod
{
	t_object x_ob;
	unsigned long decimalNumber;
	t_outlet *x_outlet1;
	t_atom hex[9];// 8 characters for 32-bit Hexadecimal Number and one for '\0'.
} t_hextod;
    /* this is called back when hextod gets a "float" message (i.e., a
    number.) */
void hextod_float(t_hextod *x, t_floatarg f)
{
	x->decimalNumber = convertToDecimal(hex);
	fflush(stdin);
    getchar();
    outlet_float(x->x_outlet1, x->decimalNumber);
}

unsigned long convertToDecimal(char hex[])
{
    char *hexString;
    int length = 0;
    const int base = 16; // Base of Hexadecimal Number
    unsigned long decimalNumber = 0;
    int i;
    // Find length of Hexadecimal Number
    for (hexString = hex; *hexString != '\0'; hexString++)
    {
        length++;
    }
    // Find Hexadecimal Number
    hexString = hex;
    for (i = 0; *hexString != '\0' && i < length; i++, hexString++)
    {
        // Compare *hexString with ASCII values
        if (*hexString >= 48 && *hexString <= 57)   // is *hexString Between 0-9
        {
            decimalNumber += (((int)(*hexString)) - 48) * pow(base, length - i - 1);
        }
        else if ((*hexString >= 65 && *hexString <= 70))   // is *hexString Between A-F
        {
            decimalNumber += (((int)(*hexString)) - 55) * pow(base, length - i - 1);
        }
        else if (*hexString >= 97 && *hexString <= 102)   // is *hexString Between a-f
        {
            decimalNumber += (((int)(*hexString)) - 87) * pow(base, length - i - 1);
        }
        else
        {
            post(" Invalid Hexadecimal Number \n");
            fflush(stdin);
            getchar();
            return 0;
            exit(0);
        }
    }
    return decimalNumber;
}
    /* this is a pointer to the class for "hextod", which is created in the
    "setup" routine below and used to create new ones in the "new" routine. */
t_class *hextod_class;

    /* this is called when a new "hextod" object is created. */
void *hextod_new(void)
{
    t_hextod *x = (t_hextod *)pd_new(hextod_class);
     x->x_outlet1 = outlet_new(&x->x_ob, &s_float);
    return (void *)x;
}

    /* this is called once at setup time, when this code is loaded into Pd. */
void hextod_setup(void)
{
    post("hextod_setup");
    hextod_class = class_new(gensym("hextod"), (t_newmethod)hextod_new, 0,
    	sizeof(t_hextod), 0, 0);
    class_addfloat(hextod_class, hextod_float);
    class_addsymbol(hextod_class, hextod_symbol);
}

