#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_PRODUCTS 100
#define MAX_BILLS 100
#define MAX_NAME_LENGTH 50
#define MAX_UNIT_LENGTH 10

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
    char unit[MAX_UNIT_LENGTH];  // Unit type (e.g., kg, liters, pieces)
} Product;

typedef struct {
    int bill_id;
    int product_ids[MAX_PRODUCTS];
    float quantities[MAX_PRODUCTS];  // To support fractional quantities for liquids/weights
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

void display_products_list() {
    printf("Existing Product List:\n");
    printf("ID   Name                Quantity    Unit     Price\n");
    printf("---------------------------------------------------\n");
    for (int i = 0; i < num_products; i++) {
        printf("%-4d %-20s %-10d %-7s %.2f\n",
               products[i].id, products[i].name, products[i].quantity,
               products[i].unit, products[i].price);
    }
}

void add_product() {
    clear_screen();
    display_products_list();  // Show product list before adding

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

        printf("Enter unit type (e.g., kg, liters, pieces): ");
        scanf("%s", new_product.unit);

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
    display_products_list();  // Show product list before deleting

    int id = get_valid_int("Enter product ID to delete (or type '0' to go back): ");
    if (id == 0) {
        printf("Returning to the main menu...\n");
        return;
    }

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
    display_products_list();  // Show product list before restocking

    int id = get_valid_int("Enter product ID to restock (or type '0' to go back): ");
    if (id == 0) {
        printf("Returning to the main menu...\n");
        return;
    }

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
    display_products_list();  // Show product list before editing

    int id = get_valid_int("Enter product ID to edit (or type '0' to go back): ");
    if (id == 0) {
        printf("Returning to the main menu...\n");
        return;
    }

    int found = 0;
    for (int i = 0; i < num_products; i++) {
        if (products[i].id == id) {
            found = 1;
            printf("Enter new name: ");
            scanf("%s", products[i].name);
            products[i].quantity = get_valid_int("Enter new quantity (>=0): ");
            products[i].price = get_valid_float("Enter new price (>=0): ");
            printf("Enter new unit type: ");
            scanf("%s", products[i].unit);
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
    display_products_list();  // Show product list before creating bill

    if (num_bills < MAX_BILLS) {
        Bill new_bill;
        new_bill.bill_id = num_bills + 1;
        new_bill.num_items = 0;
        new_bill.total_amount = 0.0;

        get_current_timestamp(new_bill.timestamp, sizeof(new_bill.timestamp));

        int cont = 1;
        while (cont) {
            char input[MAX_NAME_LENGTH];
            float quantity;
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
                        quantity = get_valid_float("Enter quantity: ");
                        if (products[i].quantity >= quantity) {
                            new_bill.product_ids[new_bill.num_items] = id;
                            new_bill.quantities[new_bill.num_items] = quantity;
                            new_bill.total_amount += quantity * products[i].price;
                            products[i].quantity -= quantity;
                            new_bill.num_items++;
                            printf("Added %s to the bill.\n", products[i].name);
                        } else {
                            printf("Not enough stock for %s.\n", products[i].name);
                        }
                        break;
                    }
                }
            } else {
                for (int i = 0; i < num_products; i++) {
                    if (strcmp(products[i].name, input) == 0) {
                        found = 1;
                        quantity = get_valid_float("Enter quantity: ");
                        if (products[i].quantity >= quantity) {
                            new_bill.product_ids[new_bill.num_items] = products[i].id;
                            new_bill.quantities[new_bill.num_items] = quantity;
                            new_bill.total_amount += quantity * products[i].price;
                            products[i].quantity -= quantity;
                            new_bill.num_items++;
                            printf("Added %s to the bill.\n", products[i].name);
                        } else {
                            printf("Not enough stock for %s.\n", products[i].name);
                        }
                        break;
                    }
                }
            }

            if (!found) {
                printf("Product not found.\n");
            }

            printf("Do you want to add more items? (1 for yes, 0 for no): ");
            scanf("%d", &cont);
        }

        bills[num_bills] = new_bill;
        num_bills++;
        save_bills();
        save_products();  // Save product changes after billing
        printf("Bill created successfully. Total: %.2f\n", new_bill.total_amount);
    } else {
        printf("Cannot create more bills. Bill storage full.\n");
    }
}

void view_bills() {
    clear_screen();
    printf("Bill List:\n");
    printf("ID   Timestamp            Total Amount\n");
    printf("--------------------------------------\n");
    for (int i = 0; i < num_bills; i++) {
        printf("%-4d %-20s %.2f\n", bills[i].bill_id, bills[i].timestamp, bills[i].total_amount);
    }
}

void main_menu() {
    int choice;
    while (1) {
        clear_screen();
        printf("1. Add Product\n");
        printf("2. Delete Product\n");
        printf("3. Restock Product\n");
        printf("4. Edit Product\n");
        printf("5. Create Bill\n");
        printf("6. View Bills\n");
        printf("7. Exit\n");
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
                restock_product();
                break;
            case 4:
                edit_product();
                break;
            case 5:
                create_bill();
                break;
            case 6:
                view_bills();
                break;
            case 7:
                exit(0);
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}

int main() {
    load_products();
    load_bills();
    main_menu();
    return 0;
}
