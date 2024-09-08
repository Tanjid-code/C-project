#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #define CLEAR_SCREEN "cls"
#else
    #define CLEAR_SCREEN "clear"
#endif

#define MAX_PRODUCTS 100
#define MAX_BILLS 100
#define MAX_NAME_LENGTH 50

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
    if (fp == NULL) {
        printf("Error saving products!\n");
        return;
    }
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
    if (fp == NULL) {
        printf("Error saving bills!\n");
        return;
    }
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

int get_valid_int(const char *prompt) {
    int input;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &input) == 1) {
            return input;
        } else {
            printf("Invalid input! Please enter a valid number.\n");
            while (getchar() != '\n'); // Clear input buffer
        }
    }
}

float get_valid_float(const char *prompt) {
    float input;
    while (1) {
        printf("%s", prompt);
        if (scanf("%f", &input) == 1) {
            return input;
        } else {
            printf("Invalid input! Please enter a valid number.\n");
            while (getchar() != '\n'); // Clear input buffer
        }
    }
}

void add_product() {
    if (num_products < MAX_PRODUCTS) {
        Product new_product;
        new_product.id = num_products + 1;
        printf("Enter product name: ");
        scanf("%s", new_product.name);
        new_product.quantity = get_valid_int("Enter quantity: ");
        new_product.price = get_valid_float("Enter price: ");

        products[num_products] = new_product;
        num_products++;

        save_products();
        printf("Product added successfully!\n");
    } else {
        printf("Product list is full!\n");
    }
}

void delete_product() {
    int id = get_valid_int("Enter product ID to delete: ");

    int found = 0;
    for (int i = 0; i < num_products; i++) {
        if (products[i].id == id) {
            found = 1;
            for (int j = i; j < num_products - 1; j++) {
                products[j] = products[j + 1];
            }
            num_products--;
            save_products();
            printf("Product deleted successfully!\n");
            break;
        }
    }
    if (!found) {
        printf("Product not found!\n");
    }
}

void edit_product() {
    int id = get_valid_int("Enter product ID to edit: ");

    int found = 0;
    for (int i = 0; i < num_products; i++) {
        if (products[i].id == id) {
            found = 1;
            printf("Enter new name: ");
            scanf("%s", products[i].name);
            products[i].quantity = get_valid_int("Enter new quantity: ");
            products[i].price = get_valid_float("Enter new price: ");
            save_products();
            printf("Product updated successfully!\n");
            break;
        }
    }
    if (!found) {
        printf("Product not found!\n");
    }
}

void create_bill() {
    if (num_bills < MAX_BILLS) {
        Bill new_bill;
        new_bill.bill_id = num_bills + 1;
        new_bill.num_items = 0;
        new_bill.total_amount = 0.0;

        int cont = 1;
        while (cont) {
            int id = get_valid_int("Enter product ID: ");
            int quantity = get_valid_int("Enter quantity: ");

            int found = 0;
            for (int i = 0; i < num_products; i++) {
                if (products[i].id == id && products[i].quantity >= quantity) {
                    new_bill.product_ids[new_bill.num_items] = id;
                    new_bill.quantities[new_bill.num_items] = quantity;
                    new_bill.total_amount += quantity * products[i].price;
                    products[i].quantity -= quantity;
                    new_bill.num_items++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("Product not available or insufficient quantity!\n");
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

void display_products() {
    printf("\nAvailable Products:\n");
    printf("--------------------\n");
    for (int i = 0; i < num_products; i++) {
        printf("ID: %d, Name: %s, Quantity: %d, Price: %.2f\n", products[i].id, products[i].name, products[i].quantity, products[i].price);
    }
}

void display_bills() {
    printf("\nBills:\n");
    printf("-------\n");
    for (int i = 0; i < num_bills; i++) {
        printf("Bill ID: %d, Total Amount: %.2f\n", bills[i].bill_id, bills[i].total_amount);
    }
}

void clear_screen() {
    system(CLEAR_SCREEN);
}

int main() {
    load_products();
    load_bills();

    int choice;

    do {
        clear_screen();  // Clear the screen before displaying the menu
        printf("\nStore Management System\n");
        printf("1. Add Product\n");
        printf("2. Delete Product\n");
        printf("3. Edit Product\n");
        printf("4. Display Products\n");
        printf("5. Create Bill\n");
        printf("6. Display Bills\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        clear_screen();  // Clear the screen before showing the output for selected option

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
                display_bills();
                break;
            case 0:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
        printf("\nPress Enter to continue...");
        getchar();  // Wait for user to press Enter
        getchar();
    } while (choice != 0);

    return 0;
}
