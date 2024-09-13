#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>  // Include time.h for date and time handling

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
    char timestamp[20]; // Field to store the time of bill creation
} Bill;

Product products[MAX_PRODUCTS];
int num_products = 0;

Bill bills[MAX_BILLS];
int num_bills = 0;

void clear_screen() {
    system(CLEAR);
}

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

int get_valid_int(const char *prompt) {
    int value;
    printf("%s", prompt);
    while (scanf("%d", &value) != 1) {
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

void create_bill() {
    if (num_bills < MAX_BILLS) {
        Bill new_bill;
        new_bill.bill_id = num_bills + 1;
        new_bill.num_items = 0;
        new_bill.total_amount = 0.0;

        // Capture current date and time
        get_current_timestamp(new_bill.timestamp, sizeof(new_bill.timestamp));

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

        // Display the bill voucher after creation
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
    if (num_products == 0) {
        printf("No products available.\n");
    } else {
        printf("Product List:\n");
        printf("ID\tName\t\tQuantity\tPrice\n");
        for (int i = 0; i < num_products; i++) {
            printf("%d\t%-15s\t%d\t\t%.2f\n", products[i].id, products[i].name, products[i].quantity, products[i].price);
        }
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
            Bill *bill_to_edit = &bills[i];

            // Display existing bill items
            printf("\nEditing Bill ID: %d, Date: %s\n", bill_to_edit->bill_id, bill_to_edit->timestamp);
            printf("Current Items:\n");
            for (int j = 0; j < bill_to_edit->num_items; j++) {
                for (int k = 0; k < num_products; k++) {
                    if (bill_to_edit->product_ids[j] == products[k].id) {
                        printf("Product: %s, Quantity: %d, Price: %.2f\n",
                               products[k].name, bill_to_edit->quantities[j], products[k].price);
                    }
                }
            }

            // Modify the items in the bill
            create_bill(); // Reuse the create_bill logic to modify the current bill
            bills[i] = bills[num_bills - 1];  // Replace the old bill with the newly created one
            num_bills--;  // Remove the extra bill that was added

            break;
        }
    }
    if (!found) {
        printf("Bill not found!\n");
    }
}

void display_bills() {
    if (num_bills == 0) {
        printf("No bills available.\n");
    } else {
        printf("Bill List:\n");
        for (int i = 0; i < num_bills; i++) {
            printf("Bill ID: %d, Total Amount: %.2f, Date: %s\n", bills[i].bill_id, bills[i].total_amount, bills[i].timestamp);
        }
    }
}

int main() {
    load_products();
    load_bills();

    int choice;

    do {
        clear_screen();
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

        printf("\nPress Enter to continue...");
        while (getchar() != '\n'); // Wait for Enter key
        getchar();
    } while (choice != 0);

    return 0;
}
