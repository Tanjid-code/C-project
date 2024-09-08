#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For isdigit()

#define MAX_PRODUCTS 100
#define MAX_BILLS 100
#define MAX_NAME_LENGTH 50

#ifdef _WIN32
    #define CLEAR "cls"  // Command for Windows
#else
    #define CLEAR "clear" // Command for Linux/Unix/MacOS
#endif

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    int quantity;
    float price;
} Product;

typedef struct {
    int bill_id;
    int product_ids[MAX_PRODUCTS];
    int quantities[MAX_PRODUCTS];
    int num_items;
    float total_amount;
} Bill;

Product products[MAX_PRODUCTS];
int num_products = 0;

Bill bills[MAX_BILLS];
int num_bills = 0;

void save_products() {
    FILE *fp = fopen("products.dat", "wb");
    fwrite(&num_products, sizeof(int), 1, fp);
    fwrite(products, sizeof(Product), num_products, fp);
    fclose(fp);
}

void load_products() {
    FILE *fp = fopen("products.dat", "rb");
    if (fp) {
        fread(&num_products, sizeof(int), 1, fp);
        fread(products, sizeof(Product), num_products, fp);
        fclose(fp);
    }
}

void save_bills() {
    FILE *fp = fopen("bills.dat", "wb");
    fwrite(&num_bills, sizeof(int), 1, fp);
    fwrite(bills, sizeof(Bill), num_bills, fp);
    fclose(fp);
}

void load_bills() {
    FILE *fp = fopen("bills.dat", "rb");
    if (fp) {
        fread(&num_bills, sizeof(int), 1, fp);
        fread(bills, sizeof(Bill), num_bills, fp);
        fclose(fp);
    }
}

void add_product() {
    if (num_products < MAX_PRODUCTS) {
        Product new_product;
        new_product.id = num_products + 1;
        printf("Enter product name: ");
        scanf("%s", new_product.name);
        printf("Enter quantity: ");
        scanf("%d", &new_product.quantity);
        printf("Enter price: ");
        scanf("%f", &new_product.price);

        products[num_products] = new_product;
        num_products++;

        save_products();
    } else {
        printf("Product list is full!\n");
    }
}

void delete_product() {
    int id;
    printf("Enter product ID to delete: ");
    scanf("%d", &id);

    int found = 0;
    for (int i = 0; i < num_products; i++) {
        if (products[i].id == id) {
            found = 1;
            for (int j = i; j < num_products - 1; j++) {
                products[j] = products[j + 1];
            }
            num_products--;
            save_products();
            break;
        }
    }
    if (!found) {
        printf("Product not found!\n");
    }
}

void edit_product() {
    int id;
    printf("Enter product ID to edit: ");
    scanf("%d", &id);

    int found = 0;
    for (int i = 0; i < num_products; i++) {
        if (products[i].id == id) {
            found = 1;
            printf("Enter new name: ");
            scanf("%s", products[i].name);
            printf("Enter new quantity: ");
            scanf("%d", &products[i].quantity);
            printf("Enter new price: ");
            scanf("%f", &products[i].price);
            save_products();
            break;
        }
    }
    if (!found) {
        printf("Product not found!\n");
    }
}

int get_valid_int(const char* prompt) {
    int val;
    printf("%s", prompt);
    scanf("%d", &val);
    return val;
}

void create_bill() {
    if (num_bills < MAX_BILLS) {
        Bill new_bill;
        new_bill.bill_id = num_bills + 1;
        new_bill.num_items = 0;
        new_bill.total_amount = 0.0;

        int cont = 1;
        while (cont) {
            char input[MAX_NAME_LENGTH];
            int quantity;
            int found = 0;

            printf("Enter product ID or name: ");
            scanf("%s", input);

            // Check if the input is a number (product ID)
            if (isdigit(input[0])) {
                int id = atoi(input); // Convert to integer

                for (int i = 0; i < num_products; i++) {
                    if (products[i].id == id) {
                        found = 1;
                        quantity = get_valid_int("Enter quantity: ");
                        if (products[i].quantity >= quantity) {
                            new_bill.product_ids[new_bill.num_items] = id;
                            new_bill.quantities[new_bill.num_items] = quantity;
                            new_bill.total_amount += quantity * products[i].price;
                            products[i].quantity -= quantity;
                            new_bill.num_items++;
                        } else {
                            printf("Insufficient stock for product ID %d!\n", id);
                        }
                        break;
                    }
                }
                if (!found) {
                    printf("Product with ID %d not found!\n", id);
                }
            }
            // Check by product name
            else {
                for (int i = 0; i < num_products; i++) {
                    if (strcmp(products[i].name, input) == 0) {
                        found = 1;
                        quantity = get_valid_int("Enter quantity: ");
                        if (products[i].quantity >= quantity) {
                            new_bill.product_ids[new_bill.num_items] = products[i].id;
                            new_bill.quantities[new_bill.num_items] = quantity;
                            new_bill.total_amount += quantity * products[i].price;
                            products[i].quantity -= quantity;
                            new_bill.num_items++;
                        } else {
                            printf("Insufficient stock for product %s!\n", products[i].name);
                        }
                        break;
                    }
                }
                if (!found) {
                    printf("Product with name '%s' not found!\n", input);
                }
            }

            cont = get_valid_int("Add more items? (1 for yes, 0 for no): ");
        }

        bills[num_bills] = new_bill;
        num_bills++;

        save_bills();
        save_products();
        printf("Bill created successfully!\n");
    } else {
        printf("Bill list is full!\n");
    }
}

void edit_bill() {
    int id;
    printf("Enter bill ID to edit: ");
    scanf("%d", &id);

    int found = 0;
    for (int i = 0; i < num_bills; i++) {
        if (bills[i].bill_id == id) {
            found = 1;
            create_bill();
            break;
        }
    }
    if (!found) {
        printf("Bill not found!\n");
    }
}

void display_products() {
    for (int i = 0; i < num_products; i++) {
        printf("ID: %d, Name: %s, Quantity: %d, Price: %.2f\n", products[i].id, products[i].name, products[i].quantity, products[i].price);
    }
}

void display_bills() {
    for (int i = 0; i < num_bills; i++) {
        printf("Bill ID: %d, Total Amount: %.2f\n", bills[i].bill_id, bills[i].total_amount);
    }
}

void clear_screen() {
    system(CLEAR);
}

int main() {
    load_products();
    load_bills();

    int choice;

    do {
        clear_screen(); // Clear screen before showing the menu
        printf("\nStore Management System\n");
        printf("1. Add Product\n");
        printf("2. Delete Product\n");
        printf("3. Edit Product\n");
        printf("4. Display Products\n");
        printf("5. Create Bill\n");
        printf("6. Edit Bill\n");
        printf("7. Display Bills\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        clear_screen(); // Clear screen after choice is entered

        switch (choice) {
            case 1:
                add_product();
                break;
            case 2:
                delete_product();
                break;
            case 3:
                edit_product();
                break;
            case 4:
                display_products();
                break;
            case 5:
                create_bill();
                break;
            case 6:
                edit_bill();
                break;
            case 7:
                display_bills();
                break;
            case 0:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
        if (choice != 0) {
            printf("\nPress Enter to continue...");
            getchar(); // Wait for user input to continue
            getchar();
        }
    } while (choice != 0);

    return 0;
}
