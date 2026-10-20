#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *source, *destination;
    char sourceFile[100], destinationFile[100];
    int ch;

    printf("Enter source file name: ");
    scanf("%99s", sourceFile);

    source = fopen(sourceFile, "r");

    if (source == NULL)
    {
        perror("Cannot open source file");
        return 1;
    }

    printf("Enter destination file name: ");
    scanf("%99s", destinationFile);

    destination = fopen(destinationFile, "w");

    if (destination == NULL)
    {
        perror("Cannot open destination file");
        fclose(source);
        return 1;
    }

    while ((ch = fgetc(source)) != EOF)
    {
        fputc(ch, destination);
    }

    fclose(source);
    fclose(destination);

    printf("\nFile copied successfully!\n");

    return 0;
}
