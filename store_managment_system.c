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
    float discount;  // Discount percentage
    char type[MAX_NAME_LENGTH]; // Product type: liquid, weight, or piece
} Product;

typedef struct {
    int bill_id;
    int product_ids[MAX_PRODUCTS];
    int quantities[MAX_PRODUCTS];
    int num_items;
    float total_amount;
    float total_tax;
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
    fwrite(&next_product_id, sizeof(int), 1, fp);
    fwrite(products, sizeof(Product), num_products, fp);
    fclose(fp);
}

void load_products() {
    FILE *fp = fopen("products.dat", "rb");
    if (fp) {
        fread(&num_products, sizeof(int), 1, fp);
        fread(&next_product_id, sizeof(int), 1, fp);
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

void display_products_list() {
    printf("Existing Product List:\n");
    printf("ID   Name                Quantity    Price     Discount   Type\n");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < num_products; i++) {
        printf("%-4d %-20s %-10d %.2f       %.2f       %s\n",
               products[i].id, products[i].name, products[i].quantity, products[i].price, products[i].discount, products[i].type);
    }
}

void add_product() {
    clear_screen();
    if (num_products < MAX_PRODUCTS) {
        Product new_product;
        new_product.id = next_product_id++;
        printf("Enter product name (or type 'back' to return): ");
        scanf("%s", new_product.name);

        if (strcmp(new_product.name, "back") == 0) {
            printf("Returning to the main menu...\n");
            return;
        }

        new_product.quantity = get_valid_int("Enter quantity (>=0): ");
        new_product.price = get_valid_float("Enter price (>=0): ");
        new_product.discount = get_valid_float("Enter discount percentage (0-100): ");
        printf("Enter product type (liquid, weight, piece): ");
        scanf("%s", new_product.type);

        products[num_products] = new_product;
        num_products++;

        save_products();
        printf("Product added successfully.\n");
    } else {
        printf("Product list is full!\n");
    }
}

void search_product() {
    clear_screen();
    char search_term[MAX_NAME_LENGTH];
    printf("Enter product name or ID to search: ");
    scanf("%s", search_term);

    printf("Search Results:\n");
    printf("ID   Name                Quantity    Price     Discount   Type\n");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < num_products; i++) {
        if (strcmp(products[i].name, search_term) == 0 || atoi(search_term) == products[i].id) {
            printf("%-4d %-20s %-10d %.2f       %.2f       %s\n",
                   products[i].id, products[i].name, products[i].quantity, products[i].price, products[i].discount, products[i].type);
        }
    }
}

void edit_product() {
    clear_screen();
    int id = get_valid_int("Enter the product ID to edit: ");
    int found = 0;

    for (int i = 0; i < num_products; i++) {
        if (products[i].id == id) {
            found = 1;
            printf("Editing product: %s\n", products[i].name);
            products[i].price = get_valid_float("Enter new price: ");
            products[i].discount = get_valid_float("Enter new discount percentage: ");
            save_products();
            printf("Product updated successfully.\n");
            break;
        }
    }

    if (!found) {
        printf("Product with ID %d not found.\n", id);
    }
}

void inventory_alerts() {
    clear_screen();
    printf("Low Stock Alerts:\n");
    printf("ID   Name                Quantity\n");
    printf("----------------------------------\n");
    for (int i = 0; i < num_products; i++) {
        if (products[i].quantity < 5) { // Threshold can be adjusted
            printf("%-4d %-20s %-10d\n", products[i].id, products[i].name, products[i].quantity);
        }
    }
}

void sales_report() {
    clear_screen();
    printf("Sales Report:\n");
    printf("Bill ID  Date and Time         Total Amount\n");
    printf("--------------------------------------------\n");
    for (int i = 0; i < num_bills; i++) {
        printf("%-8d %-20s %.2f\n",
               bills[i].bill_id, bills[i].timestamp, bills[i].total_amount);
    }
}

void create_bill() {
    clear_screen();
    if (num_bills < MAX_BILLS) {
        Bill new_bill;
        new_bill.bill_id = num_bills + 1;
        new_bill.num_items = 0;
        new_bill.total_amount = 0.0;
        new_bill.total_tax = 0.0;

        get_current_timestamp(new_bill.timestamp, sizeof(new_bill.timestamp));

        int cont = 1;
        while (cont) {
            char input[MAX_NAME_LENGTH];
            int quantity;
            int found = 0;

            printf("Enter product ID or name (or type 'back' to return): ");
            scanf("%s", input);

            if (strcmp(input, "back") == 0) {
                printf("Returning to the main menu...\n");
                return;
            }

            if (isdigit(input[0])) {
                int id = atoi(input);
                for (int i = 0; i < num_products; i++) {
                    if (products[i].id == id) {
                        found = 1;
                        quantity = get_valid_int("Enter quantity: ");
                        if (products[i].quantity >= quantity) {
                            new_bill.product_ids[new_bill.num_items] = id;
                            new_bill.quantities[new_bill.num_items] = quantity;
                            float discounted_price = products[i].price * (1 - products[i].discount / 100);
                            new_bill.total_amount += quantity * discounted_price;
                            products[i].quantity -= quantity;
                            new_bill.num_items++;
                        } else {
                            printf("Insufficient stock for product ID %d!\n", id);
                        }
                        break;
                    }
                }
            } else {
                for (int i = 0; i < num_products; i++) {
                    if (strcmp(products[i].name, input) == 0) {
                        found = 1;
                        quantity = get_valid_int("Enter quantity: ");
                        if (products[i].quantity >= quantity) {
                            new_bill.product_ids[new_bill.num_items] = products[i].id;
                            new_bill.quantities[new_bill.num_items] = quantity;
                            float discounted_price = products[i].price * (1 - products[i].discount / 100);
                            new_bill.total_amount += quantity * discounted_price;
                            products[i].quantity -= quantity;
                            new_bill.num_items++;
                        } else {
                            printf("Insufficient stock for product '%s'!\n", products[i].name);
                        }
                        break;
                    }
                }
            }

            if (!found) {
                printf("Product not found!\n");
            }

            cont = get_valid_int("Enter 1 to add another item, or 0 to finish the bill: ");
        }

        save_products();
        bills[num_bills] = new_bill;
        num_bills++;
        save_bills();
        printf("Bill created successfully!\n");
    } else {
        printf("Bill list is full!\n");
    }
}

int main() {
    load_products();
    load_bills();
    int choice;

    do {
        clear_screen();
        printf("Store Management System\n");
        printf("1. Add Product\n");
        printf("2. Search Product\n");
        printf("3. Edit Product\n");
        printf("4. Low Stock Alerts\n");
        printf("5. Create Bill\n");
        printf("6. Sales Report\n");
        printf("7. Exit\n");
        choice = get_valid_int("Choose an option: ");

        switch (choice) {
            case 1:
                add_product();
                break;
            case 2:
                search_product();
                break;
            case 3:
                edit_product();
                break;
            case 4:
                inventory_alerts();
                break;
            case 5:
                create_bill();
                break;
            case 6:
                sales_report();
                break;
            case 7:
                printf("Exiting the program. Goodbye!\n");
                break;
            default:
                printf("Invalid option. Please choose again.\n");
        }

        printf("Press Enter to continue...");
        while (getchar() != '\n'); // Clear buffer
        getchar(); // Wait for Enter
    } while (choice != 7);

    return 0;
}
