#include<stdio.h>
#include<stdlib.h>

int stringSize(char * v);

int main()
{
   char v[] = "Hola           mundo";
   int strSize;

   strSize = stringSize(v);
   printf("El string \"%s\" tiene %d caracteres.\n",v,strSize);
   printf("Escribe algo: ");
   scanf("%s",v);
   strSize = stringSize(v);
   printf("El string \"%s\" tiene %d caracteres.\n",v,strSize);
   
   if (v[0]== 'H')
   {
      printf("El primer caracter del string v es %c\n",v[0]);
   }

   return 0;
}

int stringSize(char v[] )
{
   int size = 0;
   for (int i=0; v[i]!= 0; i++)
   {
      size++;
   }
   return size;
}
