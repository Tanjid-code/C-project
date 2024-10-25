#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define ADMIN_PASSWORD "admin123"
#define EMPLOYEE_PASSWORD "employee123"
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
    float discount;
    char type[MAX_NAME_LENGTH];
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
    FILE *fp = fopen("products.csv", "w");
    if (fp) {
        fprintf(fp, "%d\n", num_products);
        fprintf(fp, "%d\n", next_product_id);
        for (int i = 0; i < num_products; i++) {
            fprintf(fp, "%d,%s,%d,%.2f,%.2f,%s\n", products[i].id, products[i].name, products[i].quantity, products[i].price, products[i].discount, products[i].type);
        }
        fclose(fp);
    } else {
        printf("Error: Unable to open file for saving products.\n");
    }
}

void load_products() {
    FILE *fp = fopen("products.csv", "r");
    if (fp) {
        fscanf(fp, "%d\n", &num_products);
        fscanf(fp, "%d\n", &next_product_id);
        for (int i = 0; i < num_products; i++) {
            fscanf(fp, "%d,%[^,],%d,%f,%f,%[^,\n]\n", &products[i].id, products[i].name, &products[i].quantity, &products[i].price, &products[i].discount, products[i].type);
        }
        fclose(fp);
    } else {
        printf("No product data found. Starting fresh.\n");
    }
}

void save_bills() {
    FILE *fp = fopen("bills.csv", "w");
    if (fp) {
        fprintf(fp, "%d\n", num_bills);
        for (int i = 0; i < num_bills; i++) {
            fprintf(fp, "%d,%d,%.2f,%s\n", bills[i].bill_id, bills[i].num_items, bills[i].total_amount, bills[i].timestamp);
            for (int j = 0; j < bills[i].num_items; j++) {
                fprintf(fp, "%d,%d\n", bills[i].product_ids[j], bills[i].quantities[j]);
            }
        }
        fclose(fp);
    } else {
        printf("Error: Unable to open file for saving bills.\n");
    }
}

void load_bills() {
    FILE *fp = fopen("bills.csv", "r");
    if (fp) {
        fscanf(fp, "%d\n", &num_bills);
        for (int i = 0; i < num_bills; i++) {
            fscanf(fp, "%d,%d,%f,%[^,\n]\n", &bills[i].bill_id, &bills[i].num_items, &bills[i].total_amount, bills[i].timestamp);
            for (int j = 0; j < bills[i].num_items; j++) {
                fscanf(fp, "%d,%d\n", &bills[i].product_ids[j], &bills[i].quantities[j]);
            }
        }
        fclose(fp);
    } else {
        printf("No bill data found. Starting fresh.\n");
    }
}

int get_valid_int(const char *prompt) {
    int value;
    printf("%s", prompt);
    while (scanf("%d", &value) != 1) {
        printf("Invalid input. Please enter a valid number: ");
        while (getchar() != '\n');
    }
    return value;
}

float get_valid_float(const char *prompt) {
    float value;
    printf("%s", prompt);
    while (scanf("%f", &value) != 1 || value < 0) {
        printf("Invalid input. Please enter a valid number: ");
        while (getchar() != '\n');
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
        printf("Enter product name: ");
        scanf("%s", new_product.name);
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
        if (products[i].quantity < 5) {
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
                            new_bill.product_ids[new_bill.num_items] = products[i].id;
                            new_bill.quantities[new_bill.num_items] = quantity;
                            new_bill.total_amount += (products[i].price * quantity) * (1 - products[i].discount / 100);
                            products[i].quantity -= quantity;
                            new_bill.num_items++;
                            save_products();
                        } else {
                            printf("Insufficient stock.\n");
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
                            new_bill.total_amount += (products[i].price * quantity) * (1 - products[i].discount / 100);
                            products[i].quantity -= quantity;
                            new_bill.num_items++;
                            save_products();
                        } else {
                            printf("Insufficient stock.\n");
                        }
                        break;
                    }
                }
            }

            if (!found) {
                printf("Product not found.\n");
            }

            printf("Do you want to add more items? (1 for Yes, 0 for No): ");
            scanf("%d", &cont);
        }

        bills[num_bills] = new_bill;
        num_bills++;
        save_bills();
        printf("Bill created successfully with Total Amount: %.2f\n", new_bill.total_amount);
    } else {
        printf("Bill list is full!\n");
    }
}

int login() {
    clear_screen();
    int role = 0;
    printf("Choose Role:\n");
    printf("1. Admin\n");
    printf("2. Employee\n");
    printf("3. Customer\n");
    printf("Enter your choice: ");
    scanf("%d", &role);

    if (role == 1) {
        char password[20];
        printf("Enter Admin Password: ");
        scanf("%s", password);
        if (strcmp(password, ADMIN_PASSWORD) == 0) {
            printf("Admin login successful.\n");
            return 1;
        } else {
            printf("Incorrect password.\n");
            return 0;
        }
    } else if (role == 2) {
        char password[20];
        printf("Enter Employee Password: ");
        scanf("%s", password);
        if (strcmp(password, EMPLOYEE_PASSWORD) == 0) {
            printf("Employee login successful.\n");
            return 2;
        } else {
            printf("Incorrect password.\n");
            return 0;
        }
    } else if (role == 3) {
        printf("Customer access granted.\n");
        return 3;
    } else {
        printf("Invalid choice.\n");
        return 0;
    }
}

void display_menu(int role) {
    printf("\n1. Display Products List\n");
    printf("2. Search Product\n");
    if (role == 1) {
        printf("3. Add Product\n");
        printf("4. Edit Product\n");
        printf("5. Inventory Alerts\n");
        printf("6. Sales Report\n");
    } else if (role == 2) {
        printf("3. Add Product\n");
        printf("4. Inventory Alerts\n");
    }
    printf("7. Create Bill\n");
    printf("0. Exit\n");
}

void main_menu(int role) {
    int choice;
    do {
        clear_screen();
        display_menu(role);
        choice = get_valid_int("Enter your choice: ");
        switch (choice) {
            case 1:
                display_products_list();
                break;
            case 2:
                search_product();
                break;
            case 3:
                if (role == 1 || role == 2) add_product();
                break;
            case 4:
                if (role == 1) edit_product();
                else if (role == 2) inventory_alerts();
                break;
            case 5:
                if (role == 1) inventory_alerts();
                break;
            case 6:
                if (role == 1) sales_report();
                break;
            case 7:
                create_bill();
                break;
            case 0:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice!\n");
        }
        printf("Press Enter to continue...");
        getchar();
        getchar();
    } while (choice != 0);
}

int main() {
    load_products();
    load_bills();

    int role = login();
    if (role != 0) {
        main_menu(role);
    } else {
        printf("Access denied.\n");
    }

    return 0;
}
