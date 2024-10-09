#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_PRODUCTS 100
#define MAX_BILLS 100
#define MAX_NAME_LENGTH 50

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
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
    char timestamp[20];
} Bill;

Product products[MAX_PRODUCTS];
int num_products = 0;
int next_product_id = 1;

Bill bills[MAX_BILLS];
int num_bills = 0;

void clear_screen() {
    system(CLEAR);
}

void save_products() {
    FILE *fp = fopen("products.dat", "wb");
    fwrite(&num_products, sizeof(int), 1, fp);
    fwrite(&next_product_id, sizeof(int), 1, fp); // Save next product ID
    fwrite(products, sizeof(Product), num_products, fp);
    fclose(fp);
}

void load_products() {
    FILE *fp = fopen("products.dat", "rb");
    if (fp) {
        fread(&num_products, sizeof(int), 1, fp);
        fread(&next_product_id, sizeof(int), 1, fp); // Load next product ID
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

int get_valid_int(const char *prompt) {
    int value;
    printf("%s", prompt);
    while (scanf("%d", &value) != 1) {
        printf("Invalid input. Please enter a valid number: ");
        while (getchar() != '\n'); // Clear invalid input
    }
    return value;
}

float get_valid_float(const char *prompt) {
    float value;
    printf("%s", prompt);
    while (scanf("%f", &value) != 1 || value < 0) {
        printf("Invalid input. Please enter a valid number: ");
        while (getchar() != '\n'); // Clear invalid input
    }
    return value;
}

void get_current_timestamp(char *buffer, int length) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, length, "%Y-%m-%d %H:%M:%S", t);
}

void add_product() {
    clear_screen();
    if (num_products < MAX_PRODUCTS) {
        Product new_product;
        new_product.id = next_product_id++;
        printf("Enter product name: ");
        scanf("%s", new_product.name);
        new_product.quantity = get_valid_int("Enter quantity (>=0): ");
        new_product.price = get_valid_float("Enter price (>=0): ");

        products[num_products] = new_product;
        num_products++;

        save_products();
        printf("Product added successfully.\n");
    } else {
        printf("Product list is full!\n");
    }
}

void delete_product() {
    clear_screen();
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
            printf("Product deleted successfully.\n");
            break;
        }
    }
    if (!found) {
        printf("Product not found!\n");
    }
}

void restock_product() {
    clear_screen();
    int id = get_valid_int("Enter product ID to restock: ");

    int found = 0;
    for (int i = 0; i < num_products; i++) {
        if (products[i].id == id) {
            found = 1;
            int quantity = get_valid_int("Enter quantity to add: ");
            if (quantity >= 0) {
                products[i].quantity += quantity;
                save_products();
                printf("Product restocked successfully.\n");
            } else {
                printf("Invalid quantity!\n");
            }
            break;
        }
    }
    if (!found) {
        printf("Product not found!\n");
    }
}

void edit_product() {
    clear_screen();
    int id = get_valid_int("Enter product ID to edit: ");

    int found = 0;
    for (int i = 0; i < num_products; i++) {
        if (products[i].id == id) {
            found = 1;
            printf("Enter new name: ");
            scanf("%s", products[i].name);
            products[i].quantity = get_valid_int("Enter new quantity (>=0): ");
            products[i].price = get_valid_float("Enter new price (>=0): ");
            save_products();
            printf("Product updated successfully.\n");
            break;
        }
    }
    if (!found) {
        printf("Product not found!\n");
    }
}

void create_bill() {
    clear_screen();
    if (num_bills < MAX_BILLS) {
        Bill new_bill;
        new_bill.bill_id = num_bills + 1;
        new_bill.num_items = 0;
        new_bill.total_amount = 0.0;

        get_current_timestamp(new_bill.timestamp, sizeof(new_bill.timestamp));

        int cont = 1;
        while (cont) {
            char input[MAX_NAME_LENGTH];
            int quantity;
            int found = 0;

            printf("Enter product ID or name: ");
            scanf("%s", input);

            if (isdigit(input[0])) {
                int id = atoi(input);

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
            } else {
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

        printf("\n--- Bill Voucher ---\n");
        printf("Bill ID: %d\n", new_bill.bill_id);
        printf("Date and Time: %s\n", new_bill.timestamp);
        printf("Items Purchased:\n");
        for (int i = 0; i < new_bill.num_items; i++) {
            for (int j = 0; j < num_products; j++) {
                if (new_bill.product_ids[i] == products[j].id) {
                    printf("Product: %s, Quantity: %d, Price: %.2f\n",
                        products[j].name, new_bill.quantities[i], products[j].price);
                    break;
                }
            }
        }
        printf("Total Amount: %.2f\n", new_bill.total_amount);
        printf("----------------------\n");
    } else {
        printf("Bill list is full!\n");
    }
}

void display_products() {
    clear_screen();
    if (num_products == 0) {
        printf("No products available.\n");
    } else {
        printf("Product List:\n");
        printf("ID    Name                Quantity    Price\n");
        printf("--------------------------------------------\n");
        for (int i = 0; i < num_products; i++) {
            printf("%-5d %-20s %-10d %-10.2f\n",
                products[i].id, products[i].name, products[i].quantity, products[i].price);
        }
    }
}

void display_bills() {
    clear_screen();
    if (num_bills == 0) {
        printf("No bills available.\n");
    } else {
        printf("Bill List:\n");
        printf("Bill ID  Date and Time         Total Amount\n");
        printf("--------------------------------------------\n");
        for (int i = 0; i < num_bills; i++) {
            printf("%-8d %-20s %-10.2f\n",
                bills[i].bill_id, bills[i].timestamp, bills[i].total_amount);
        }
    }
}

void edit_bill() {
    clear_screen();
    int bill_id = get_valid_int("Enter bill ID to edit: ");
    int found = 0;

    for (int i = 0; i < num_bills; i++) {
        if (bills[i].bill_id == bill_id) {
            found = 1;
            printf("Editing Bill ID %d...\n", bill_id);
            for (int j = 0; j < bills[i].num_items; j++) {
                int product_id = bills[i].product_ids[j];
                int new_quantity = get_valid_int("Enter new quantity (0 to remove): ");
                if (new_quantity == 0) {
                    // Remove item
                    for (int k = j; k < bills[i].num_items - 1; k++) {
                        bills[i].product_ids[k] = bills[i].product_ids[k + 1];
                        bills[i].quantities[k] = bills[i].quantities[k + 1];
                    }
                    bills[i].num_items--;
                } else {
                    bills[i].quantities[j] = new_quantity;
                }
            }
            save_bills();
            printf("Bill edited successfully.\n");
            break;
        }
    }

    if (!found) {
        printf("Bill not found!\n");
    }
}

int main() {
    load_products();
    load_bills();

    int choice;
    do {
        printf("\n--- Store Management System ---\n");
        printf("1. Add Product\n");
        printf("2. Delete Product\n");
        printf("3. Edit Product\n");
        printf("4. Restock Product\n");
        printf("5. Create Bill\n");
        printf("6. Display Products\n");
        printf("7. Display Bills\n");
        printf("8. Edit Bill\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");
        choice = get_valid_int("");

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
            restock_product();
            break;
        case 5:
            create_bill();
            break;
        case 6:
            display_products();
            break;
        case 7:
            display_bills();
            break;
        case 8:
            edit_bill();
            break;
        case 9:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice!\n");
        }
    } while (choice != 9);

    return 0;
}
