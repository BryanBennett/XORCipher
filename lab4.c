/**
 * This program reads a file and either encrypts or decrypts it with a key, depending on the flags provided.
 * The resultant encrypted or decrypted messages are printed into a new file
 *
 * @author Bryan Bennett {@literal <bennbc16@wfu.edu>}
 * @date Oct. 12, 2020
 * @assignment Lab 4
 * @course CSC 250
 **/

#include "get_args.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Function Prototypes*/
int encryptMaster(FILE *fptrR, FILE *fptrW, unsigned int key);
int decryptMaster(FILE *fptrR, FILE *fptrW, unsigned int key);
unsigned int encrypt(unsigned int key, unsigned char *string);
void decrypt(unsigned int key, unsigned int value, unsigned char *string);
short int numCharsDecrypt(unsigned int value, unsigned int key);

/*Function Definitions*/

/* MAIN: largely focuses on determining if encrypt or decrypt is indicated and then calling the appropriate functions */
int main(int argc, char *argv[])
{
    /* Variable Declarations */
    int argsOK;                                                                     /* Returns a 1 if no errors with process_args */
    int enc;                                                                        /* process_args returns 1=encrypt, 0=decrypt */
    int key;                                                                        /* process_args returns key for enc or dec */
    char fileNameOriginal[20];                                                      /* process_args returns file name from command line */
    FILE *fptrR = NULL;                                                             /* READ file ptr */
    FILE *fptrW = NULL;                                                             /* WRITE file ptr */
    int totalNumChars = 0;                                                          /* Will be used to track number of chars encrypted/decrypted */
    char fileNameWrite[25];                                                         /* Char array, stores write-to file name */
    char fileNameRead[25];                                                          /* Char array, stores read-from file name */

    argsOK = process_args(argc, argv, &enc, &key, fileNameOriginal);                /* Parses command line, initializes enc, key, and fileNameOriginal */
    memcpy(fileNameRead, fileNameOriginal, strlen(fileNameOriginal)+1);             /* Copies original file name to fileNameRead */
    memcpy(fileNameWrite, fileNameOriginal, strlen(fileNameOriginal)+1);            /* Copies original file name to fileNameWrite */
    fptrR = fopen(fileNameRead, "re");                                              /* Points read-from file pointer to fileNameRead */

    /* Error checking */
    if ((key < 0) || (key > 255)) {                                                 /* Ensures key is 8 bits */
        printf("Key = %d, which is not between 0 and 255 \n", key);
        return 1;
    }
    if (argsOK != 1) {                                                              /* Ensures process_args returned without errors */
        printf("Error, process_args returned error \n");
        return 1;
    }                                                       
    if (argc < 5) {
        printf("Usage: %s filename \n", argv[0]);                                   /* Ensures enough arguments were sent via command line */
        return 1;
    }
    if (fptrR == NULL) {
        printf("Read file does not exist or cannot be read \n");                     /* Ensures read-from file can be opened and read */
        return 1;
    }

    /* Checks if encrypt or decrypt, executes proper functions */
    if (enc == 1) {                                                                 /* Enc == 1, call encrpyt */
        strncat(fileNameWrite, ".enc", 5);                                          /* Append .enc to write-to file name */
        fptrW = fopen(fileNameWrite, "we");                                         /* Create this new file and point fptrW to it */
        totalNumChars = encryptMaster(fptrR, fptrW, key);                           /* Call encyptMaster function, which handles encryption and returns char count */
        printf("encrypted %d characters in %s\n", totalNumChars, fileNameRead);     /* Print statements */
        printf("encypted file is %s\n", fileNameWrite);
    }
    else if (enc == 0) {                                                            /* Enc == 0, call decrypt */
        strncat(fileNameWrite, ".dec", 5);                                          /* Append .dec to write-to file name */
        fptrW = fopen(fileNameWrite, "we");                                         /* Create this new file and point fptrW to it */
        totalNumChars = decryptMaster(fptrR, fptrW, key);                           /* Call decryptMaster function, which handles decryption and returns char count */
        printf("decrypted %d characters in %s\n", totalNumChars, fileNameRead);     /* Print statements */
        printf("decypted file is %s\n", fileNameWrite);
    }
    else {                                                                          /* Neither enc or dec triggered, throw error */
        printf("ERROR, ECRYPTION/DECRYPTION CHOICE NOT FOUND");
        return 1;
    }

    fclose(fptrR);                                                                  /* Close write-to and read-from files */
    fclose(fptrW);

    return 0;
}


/* Method that acts as the "conductor" of the encryption process. Navigates chars in txt file, sends chunks to encrypt and prints to .enc file*/
int encryptMaster(FILE *fptrR, FILE *fptrW, unsigned int key)
{
    short int i = 0;                                            /* Used to navigate remaining string after while */
    short int counter = 0;                                      /* Counts number of chars stored in array */
    unsigned char string[5];                                    /* Stores chars collected from .txt file */
    unsigned int value;                                         /* Stores int which represents encrpyted chars */
    int totalNumChars = 0;                                      /* Tracks total quantity of characters encrypted */
    unsigned char c = fgetc(fptrR);                             /* Stores each individual char from .txt file inside loop */
    string[4] = '\0';
    

    /* Navigates through input file, chunks chars into array and sends them to be encrypted */
    while (!feof(fptrR))
    {

        if (counter < 4) {                                      /* Array not full, add new char into string and increment count */
            string[counter] = c;
            counter++;
            c = fgetc(fptrR);
        }

        else if (counter == 4) {                                /* Array full (4 chars). Encrypt and print uint to new file */
            if (totalNumChars != 0) {
                fprintf(fptrW, "\n");
            }
            value = encrypt(key, string);
            fprintf(fptrW, "%u", value);
            counter = 0;                                        /* Reset counter */
            totalNumChars = totalNumChars + 4;                  /* Four new characters added */
        }
    }

    /* If there are remaining chars and array did not fill before end of file, encrypt them now*/
    if (counter > 0) {
        for ( i = counter; i < 4; i++) {                        /* Fill remaining array with 0s */
            string[i] = 0;
        }
        value = encrypt(key, string);
        fprintf(fptrW, "\n%u", value);
        totalNumChars = totalNumChars + counter;                /* "counter" new characters added */
    }

    return totalNumChars;
}


/* Method that acts as the "conductor" of the decryption process. Navigates .enc file, sends u-ints to decrypt and prints resultant string to .dec */
int decryptMaster(FILE *fptrR, FILE *fptrW, unsigned int key)
{
    short int counter = 4;                                      /* Represents the number of chars that went into making an encrypted uint */
    unsigned char string[5];                                    /* Stores chars decrypted from each uint in .enc file */
    unsigned int value;                                         /* Stores uint from .enc file which represents encrpyted chars */
    int totalNumChars = 0;                                      /* Tracks total quantity of characters decrypted */
    int notLastInt;                                             /* Used to determine if more integers exist in .enc. 1 = more ints are left */
    unsigned int tmpValue;                                      /* Stores next int, so it can be checked for EOF */

    string[4] = '\0';                                           /* Indicate end of string */
    notLastInt = fscanf(fptrR, "%u", &value);                   /* Initialize "value" by pulling in first uint */

    /* Navigates .enc file, pulls in each uint and decrypts. Prints string to .dec file 
    Before doing so, it checks the next integer in the file. If no assignment is made to 
    tmpValue, we known "value" is the last integer and therefore could be fewer than 4 chars
    This is better as it ensures correct char counts even if the key is in the last byte of the 
    uint in the text body (because it only checks last integer), which would cause it to erroneously 
    count nothing when XORd */

    while (notLastInt == 1) {                                   /* While there are more ints left */
    
        if ( fscanf(fptrR, "%u", &tmpValue) != 1 ) {            /* Check the next int, if there isn't one than "value" is the last one */
            counter = numCharsDecrypt(value, key);
            notLastInt = -1;
        }                       
        decrypt(key, value, string);
        fprintf(fptrW, "%s", string);
        totalNumChars = totalNumChars + counter;
        value = tmpValue;

    }

    return totalNumChars;
}


/* Encypts an individual chunk of four characters, returns the unsigned int */
unsigned int encrypt(unsigned int key, unsigned char *string)
{
    unsigned int result = 0;
    unsigned int i = 0;
    unsigned int intArray[4] = {0};

    /* Navigates string and stores each XORd byte as its own integer, which is then ORd with "result" */
    for (i = 0; i < 4; i++) {
        intArray[i] = ( string[i] ^ key ) << ( (3u - i) * 8u );             /* XOR char w key, then shift according to string index */
        result = ( result | intArray[i] );
        string[i] = 0;
    }

    return result;
}


/* Decrypts an individual unsigned integer, ALWAYS returs an array of chars of length 4 */
void decrypt(unsigned int key, unsigned int value, unsigned char *string)
{
    unsigned int i = 0;

    /* Bitshift value based on char position, XOR each byte with key and cast as char */
    for (i = 0; i < 4; i++) {
        string[i] = ((unsigned char)((value >> ((3u - i) * 8u)) & 255u)) ^ key;
    }

}


/* Checks how many characters got encrypted into an unsigned integer by checking if each byte is non-zero or zero (starts from the LSB) */
short int numCharsDecrypt(unsigned int value, unsigned int key)
{
    unsigned int fourChars = 0x000000FF;
    unsigned int threeChars = 0x0000FF00;
    unsigned int twoChars = 0x00FF0000;
    unsigned int oneChar = 0xFF000000;
    short int counter;
    unsigned int i = 0;

    /* XORs each byte with key */
    for (i = 0; i < 4; i++) {
        value = value ^ (key << ((3u - i) * 8u));
    }

    /* Checks bytes beginning from the right, this reduces erronous counts where key is not in least-sig byte of a 4-char array */ 
    if ( ( ( value & fourChars ) ) != 0) {
        counter = 4;
    }
    else if ( ( ( value & threeChars ) ) != 0) {
        counter = 3;
    }
    else if ( ( ( value & twoChars ) ) != 0) {
        counter = 2;
    }
    else if ( ( ( value & oneChar ) ) != 0) {
        counter = 1;
    }
    else {
        printf("Error, zero-integer attempted to convert to char array");
    }

    return counter;
}
