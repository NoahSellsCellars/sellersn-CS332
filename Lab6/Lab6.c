#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LINESIZE 1024

//Define the struct to hold the data
struct listing {
    int id, host_id, minimum_nights, number_of_reviews, calculated_host_listings_count, availability_365;
    
    char *host_name, *neighbourhood_group, *neighbourhood, *room_type;
    
    float latitude, longitude, price;
};

struct listing get_fields(char *line) {
    struct listing item;
    item.id = atoi(strtok(line, ","));
    item.host_id = atoi(strtok(NULL, ","));
    item.host_name = strdup(strtok(NULL, ","));
    item.neighbourhood_group = strdup(strtok(NULL, ","));
    item.neighbourhood = strdup(strtok(NULL, ","));
    item.latitude = atof(strtok(NULL, ","));
    item.longitude = atof(strtok(NULL, ","));
    item.room_type = strdup(strtok(NULL, ","));
    item.price = atof(strtok(NULL, ","));
    item.minimum_nights = atoi(strtok(NULL, ","));
    item.number_of_reviews = atoi(strtok(NULL, ","));
    item.calculated_host_listings_count = atoi(strtok(NULL, ","));
    item.availability_365 = atoi(strtok(NULL, ","));
    return item;
}
int compare_host_names(const void *a, const void *b) {
    struct listing *listing_a = (struct listing *)a;
    struct listing *listing_b = (struct listing *)b;
    return strcmp(listing_a->host_name,listing_b->host_name);
}

int compare_price(const void *a, const void *b) {
    struct listing *listing_a = (struct listing *)a;
    struct listing *listing_b = (struct listing *)b;
    if (listing_a->price < listing_b->price) {
        return -1;
    }
    if (listing_a->price > listing_b->price) {
        return 1;
    }
    return 0;
}

char* displayStruct_string_builder(struct listing item) {
    const int BUFSIZE = 1024;
    char *build_string = malloc(BUFSIZE * sizeof(char));
    if (build_string == NULL) {
        fprintf(stderr, "Memory allocation failed");
    }
    snprintf(build_string, BUFSIZE,  "ID : %d\nHost ID : %d\nHost Name : %s\nNeighbourhood Group : %s\nNeighbourhood : %s\nLatitude : %f\nLongitude : %f\nRoom Type : %s\nPrice : %f\nMinimum Nights : %d\nNumber of Reviews : %d\nCalculated Host Listings Count : %d\nAvailability_365 : %d\n",
    item.id, item.host_id, item.host_name, item.neighbourhood_group, item.neighbourhood, item.latitude, item.longitude, item.room_type, item.price, item.minimum_nights, item.number_of_reviews, item.calculated_host_listings_count, item.availability_365);
    /*printf("ID : %d\n", item.id);
    printf("Host ID : %d\n", item.host_id);
    printf("Host Name : %s\n", item.host_name);
    printf("Neighbourhood Group : %s\n", item.neighbourhood_group);
    printf("Neighbourhood : %s\n", item.neighbourhood);
    printf("Latitude : %f\n", item.latitude);
    printf("Longitude : %f\n", item.longitude);
    printf("Room Type : %s\n", item.room_type);
    printf("Price : %f\n", item.price);
    printf("Minimum Nights : %d\n", item.minimum_nights);
    printf("Number of Reviews : %d\n", item.number_of_reviews);
    printf("Calculated Host Listings Count : %d\n",
    item.calculated_host_listings_count);
    printf("Availability_365 : %d\n\n", item.availability_365);*/
    return build_string;
}

int main(int argc, char *argv[]) {
    /*This program reads a csv file, uses structs to store line data, and displays
    the sorted data
    
    Takes:
    A relative or absolute path to a csv file
    
    Side effects:
    Prints the sorted line data
    */

    if (argc != 4) {
        printf("Usage %s <path/to/csv> <path/to/output1> <path/to/output2>\n", argv[0]);
        exit(-1);
    }
   
    struct listing list_items[22555];
    char line[LINESIZE];
    char *filepath = argv[1];
    int count;
    FILE *fptr, *fpout_host_name, *fpout_price;

    //Open the file
    if ((fptr = fopen(filepath, "r")) == NULL) {
        fprintf(stderr, "Error opening file %s\n", argv[1]);
    }
    //Get the line data
    count = 0;
    while (fgets(line, LINESIZE, fptr) != NULL) {
        list_items[count] = get_fields(line);
        count++;
    }
    //At the end of the loop, count will point to the index of the last line + 1

    //Sort the lines by host_name
    struct listing *host_name_sorted_list_items;
    host_name_sorted_list_items = malloc(sizeof(list_items));
    memcpy(host_name_sorted_list_items, &list_items, sizeof(list_items));

    qsort(host_name_sorted_list_items, count, sizeof(list_items[0]), compare_host_names);

    //Create a new file
    if ((fpout_host_name = fopen(argv[2], "w")) == NULL) {
        fprintf(stderr, "Error opening file %s\n", argv[2]);
    }
    //Write the contents from host_name_sorted_list_items to the file
    for(int i = 0; i < count; i++) {
        char *output_string = displayStruct_string_builder(host_name_sorted_list_items[i]);
        strcat(output_string, "\n");
        fputs(output_string, fpout_host_name);
        free(output_string);
    }

    //Sort the lines by price
    struct listing *price_sorted_list_items;
    price_sorted_list_items = malloc(sizeof(list_items));
    memcpy(price_sorted_list_items, &list_items, sizeof(list_items));

    qsort(price_sorted_list_items, count, sizeof(list_items[0]), compare_price);

    //New file
    if ((fpout_price = fopen(argv[3], "w")) == NULL) {
        fprintf(stderr, "Error opening file %s\n", argv[3]);
    }
    //Write the contents from price_sorted_list_items to the file
    for(int i = 0; i < count; i++) {
        char *output_string = displayStruct_string_builder(price_sorted_list_items[i]);
        strcat(output_string, "\n");
        fputs(output_string, fpout_price);
        free(output_string);
    }

    //Close the files
    fclose(fptr);
    fclose(fpout_host_name);
    fclose(fpout_price);

    //Clean up
    free(host_name_sorted_list_items);
    free(price_sorted_list_items);
    return 0;
}
