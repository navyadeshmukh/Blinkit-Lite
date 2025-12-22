#include <stdio.h>
#include <string.h>

#define MAX_SKU 3000
#define MAX_ORD 5000
#define MAX_ITEMS 20
#define SUCCESS 1
#define FAILURE 0

int skuCount = 0;
int orderCount = 0;
int next_order_id = 1001;

struct SKU {
    int skuId;         
    char name[50];
    ........................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................
    char category[50];
    float price;
    int stock;
    int soldCount;
};

struct Order {
    int orderId;       
    long long int orderTime;     
    int status;        //0=Placed,1=Delivered,2=Cancelled
    long long int deliveryTime;  
    int itemIds[MAX_ITEMS];
    int itemQty[MAX_ITEMS];
    int itemCount;
    float subtotal;
};

struct SKU SKUList[MAX_SKU];
struct Order OrderList[MAX_ORD];


int findSKUIndex(int skuId) {
    for(int i = 0; i < skuCount; i++) {
        if(SKUList[i].skuId == skuId) return i;
    }
    return -1;
}

void Add_SKU() {
    int status_code = SUCCESS;
    if(skuCount >= MAX_SKU){
        printf("Cannot add more SKUs\n");
        status_code = FAILURE;
    }
    else {
        struct SKU new_sku;
        printf("Enter SKU ID: ");
        scanf("%d", &new_sku.skuId);
    
        if(findSKUIndex(new_sku.skuId)!= -1) {
            printf("SKU ID already exists\n");
            status_code = FAILURE;
        }
        if(status_code) {
            printf("Enter Name: ");
            scanf("%s", new_sku.name);
            printf("Enter Category: ");
            scanf("%s", new_sku.category);
            printf("Enter Price: "); 
            scanf("%f", &new_sku.price);
            printf("Enter Stock: ");
            scanf("%d", &new_sku.stock);
            if(new_sku.price < 0 || new_sku.stock < 0) {
                printf("Price and Stock must be non-negative\n");
                status_code = FAILURE;
            }
            new_sku.soldCount = 0;
            SKUList[skuCount] = new_sku;
            skuCount++;
        }
    }
    if(!status_code) printf("SKU could not be added.\n");
    else printf("SKU added successfully!\n");

}

void Update_or_Delete_SKU(int skuId, int action) {
    
    int status_code = SUCCESS;
    int idx = findSKUIndex(skuId);
    if (idx == -1) {
        printf("Error: SKU not found!\n");
        status_code = FAILURE;
    }
    if (action == 0 && status_code) {  // DELETE
        // Check if SKU is referenced in any delivered order
        for (int i = 0; i < orderCount; i++) {
            if (OrderList[i].status == 1) {  // 1 = Delivered
                for (int j = 0; j < OrderList[i].itemCount; j++) {
                    if (OrderList[i].itemIds[j] == skuId) {
                        printf("Cannot delete SKU %d — referenced in a delivered order.\n", skuId);
                        status_code = FAILURE;
                    }
                }
            }
        }
        if (status_code) {
            for (int i = idx; i < skuCount - 1; i++) {
                SKUList[i] = SKUList[i + 1];
            }
            skuCount--;
            printf("SKU %d deleted successfully.\n", skuId);
        }
    }

    else if (action == 1 && status_code) {  // UPDATE
        struct SKU *sku = &SKUList[idx];
        char newName[50], newCategory[50];
        float newPrice;
        int newStock;

        printf("Enter new Name (current: %s): ", sku->name);
        scanf("%49s", newName);
        printf("Enter new Category (current: %s): ", sku->category);
        scanf("%49s", newCategory);
        printf("Enter new Price (current: %.2f): ", sku->price);
        scanf("%f", &newPrice);
        printf("Enter new Stock (current: %d): ", sku->stock);
        scanf("%d", &newStock);

        if (newPrice < 0 || newStock < 0) {
            printf("Error: Price and Stock must be non-negative!\n");
            status_code = FAILURE;
        }
        if(status_code) {
            strcpy(sku->name, newName);
            strcpy(sku->category, newCategory);
            sku->price = newPrice;
            sku->stock = newStock;

            printf("SKU %d updated successfully.\n", skuId);
        }
    }

    else {
        printf("Invalid action.\n");
    }
}

void Place_Order(int itemIds[], int itemQty[], int itemCount, long long int orderTime) {
    
    if (orderCount >= MAX_ORD) {
        printf("Cannot place more orders\n");
        return;
    }

    struct Order new_order;
    new_order.orderId = next_order_id++;
    new_order.orderTime = orderTime;
    new_order.status = 0; // Placed
    new_order.deliveryTime = 0;
    new_order.itemCount = itemCount;
    new_order.subtotal = 0.0f;

    int validCount = 0;
    
    for (int i = 0; i < itemCount; i++) {
        int status_code = SUCCESS;

        int sku_idx = findSKUIndex(itemIds[i]);
        
        if (sku_idx == -1) {
            printf("Item ID %d not found. Skipping.\n", itemIds[i]);
            status_code = FAILURE;
            continue;
        }
        
        if (itemQty[i] <= 0) {
            printf("Invalid quantity for item ID %d.\n", itemIds[i]);
            status_code = FAILURE;
            continue;
        }
        
        if (SKUList[sku_idx].stock == 0) {
            printf("Item '%s' currently not available.\n", SKUList[sku_idx].name);
            status_code = FAILURE;
            continue;
        }
        

        if (itemQty[i] > SKUList[sku_idx].stock) {
            printf("Only %d units of '%s' available.\n",
                SKUList[sku_idx].stock, SKUList[sku_idx].name);
                
                printf("Do you want to order available quantity (%d)? (1=Yes / 0=No): ",
                SKUList[sku_idx].stock);
                    
                int choice;
                scanf("%d", &choice);

                if (choice == 1) {
                    itemQty[i] = SKUList[sku_idx].stock;
                }
                else {
                    printf("Item '%s' not added to order.\n", SKUList[sku_idx].name);
                    status_code = FAILURE;
                }
        }
        if(status_code) {
            new_order.itemIds[validCount] = itemIds[i];
            new_order.itemQty[validCount] = itemQty[i];
            new_order.subtotal += SKUList[sku_idx].price * itemQty[i];
            validCount++;
            
            printf("Added '%s' (Qty: %d, Price: %.2f, Total: %.2f)\n",
                SKUList[sku_idx].name, itemQty[i], 
                SKUList[sku_idx].price, new_order.subtotal);
        }
            
    }
    new_order.itemCount = validCount;

    if (validCount > 0 && new_order.subtotal > 0) {
        OrderList[orderCount] = new_order;
        orderCount++;

        printf("Total Amount to be paid for your order: ₹%.2f\n", new_order.subtotal);
        printf("Order placed successfully!\n");
        printf("Your Order ID is: %d\n", new_order.orderId);
    }
    else {
        printf("Order could not be placed.\n");
    }
}

void Deliver_Order(int orderId, long long int deliveryTime) {
    int order_idx = -1;

    for (int i = 0; i < orderCount; i++) {
        if (OrderList[i].orderId == orderId) {
            order_idx = i;
            break;
        }
    }

    if (order_idx == -1) {
        printf("Error: Order ID %d not found.\n", orderId);
        return;
    }

    
    if (OrderList[order_idx].status == 1) {
        printf("Order #%d has already been delivered.\n", orderId);
        return;
    } 
    else if (OrderList[order_idx].status == 2) {
        printf("Order #%d was cancelled. Cannot deliver.\n", orderId);
        return;
    }

    // Re-validate stock for each item
    for (int i = 0; i < OrderList[order_idx].itemCount; i++) {
        int sku_idx = findSKUIndex(OrderList[order_idx].itemIds[i]);
        if (sku_idx == -1) {
            printf("Error: SKU ID %d not found in catalog. Delivery aborted.\n",
                   OrderList[order_idx].itemIds[i]);
            return;
        }

        if (SKUList[sku_idx].stock < OrderList[order_idx].itemQty[i]) {
            printf("Insufficient stock for '%s'. Available: %d, Required: %d.\n",
                   SKUList[sku_idx].name,
                   SKUList[sku_idx].stock,
                   OrderList[order_idx].itemQty[i]);
            printf("Delivery aborted.\n");
            return;
        }
    }

    for (int i = 0; i < OrderList[order_idx].itemCount; i++) {
        int sku_idx = findSKUIndex(OrderList[order_idx].itemIds[i]);
        SKUList[sku_idx].stock -= OrderList[order_idx].itemQty[i];
        SKUList[sku_idx].soldCount += OrderList[order_idx].itemQty[i];
    }

    OrderList[order_idx].status = 1;              // Delivered
    OrderList[order_idx].deliveryTime = deliveryTime;

    printf("\nOrder #%d delivered successfully at %lld!\n", 
           OrderList[order_idx].orderId, deliveryTime);
    printf("Items Delivered:\n");

    for (int i = 0; i < OrderList[order_idx].itemCount; i++) {
        int sku_idx = findSKUIndex(OrderList[order_idx].itemIds[i]);
        printf("   - %s | Qty: %d | Remaining Stock: %d | Total Sold: %d\n",
               SKUList[sku_idx].name,
               OrderList[order_idx].itemQty[i],
               SKUList[sku_idx].stock,
               SKUList[sku_idx].soldCount);
    }

    printf("Order Total: ₹%.2f\n", OrderList[order_idx].subtotal);
}

void Cancel_Order(int orderId) {
    int order_idx = -1;

    for (int i = 0; i < orderCount; i++) {
        if (OrderList[i].orderId == orderId) {
            order_idx = i;
            break;
        }
    }

    if (order_idx == -1) {
        printf("Error: Order ID %d not found.\n", orderId);
        return;
    }

    if (OrderList[order_idx].status == 1) {
        printf("Order #%d has already been delivered. Cannot cancel.\n", orderId);
        return;
    } 
    else if (OrderList[order_idx].status == 2) {
        printf("Order #%d is already cancelled.\n", orderId);
        return;
    }

    OrderList[order_idx].status = 2;  // Cancelled
    printf("Order #%d has been cancelled successfully.\n", orderId);
}

char toLowerChar(char c) {
    if (c >= 'A' && c <= 'Z')
        return c + ('a' - 'A');
    return c;
}

void Search_SKU_ByName_Substring(char query[]) {
    int found = 0;
    char lowerQuery[50];
    strcpy(lowerQuery, query);

    for (int i = 0; lowerQuery[i]; i++) {
        lowerQuery[i] = toLowerChar(lowerQuery[i]);
    }

    printf("\nSearch results for '%s':\n", query);

    for (int i = 0; i < skuCount; i++) {
        char lowerName[50];
        strcpy(lowerName, SKUList[i].name);

        for (int j = 0; lowerName[j]; j++) {
            lowerName[j] = toLowerChar(lowerName[j]);
        }

        if (strstr(lowerName, lowerQuery) != NULL) {
            printf(" [%d] %s | Category: %s | Price: %.2f | Stock: %d | Sold: %d\n",
                   SKUList[i].skuId, SKUList[i].name, SKUList[i].category,
                   SKUList[i].price, SKUList[i].stock, SKUList[i].soldCount);
            found = 1;
        }
    }
    if (!found) {
        printf("No SKU found matching '%s'\n", query);
    }
}

void Sort_Orders_ByTime() {
    if (orderCount <= 1) {
        printf("Not enough orders to sort.\n");
        return;
    }

    for (int i = 0; i < orderCount - 1; i++) {
        for (int j = 0; j < orderCount - i - 1; j++) {
            if ((OrderList[j].orderTime > OrderList[j + 1].orderTime) ||
                (OrderList[j].orderTime == OrderList[j + 1].orderTime &&
                 OrderList[j].orderId > OrderList[j + 1].orderId)) {

                struct Order temp = OrderList[j];
                OrderList[j] = OrderList[j + 1];
                OrderList[j + 1] = temp;
            }
        }
    }

    printf("\nOrders sorted successfully (by time).\n");
    for (int i = 0; i < orderCount; i++) {
        printf("   #%d | Time: %lld | Status: %d | Subtotal: ₹%.2f\n",
               OrderList[i].orderId, OrderList[i].orderTime,
               OrderList[i].status, OrderList[i].subtotal);
    }
}

void TopK_Bestsellers(int K) {
    if (skuCount == 0) {
        printf("No SKUs in the catalog.\n");
        return;
    }

    if (K > skuCount) K = skuCount; 

    int idx[MAX_SKU];
    for (int i = 0; i < skuCount; i++) idx[i] = i;

    for (int i = 0; i < skuCount - 1; i++) {
        for (int j = 0; j < skuCount - i - 1; j++) {
            int a = idx[j], b = idx[j + 1];
            if ((SKUList[a].soldCount < SKUList[b].soldCount) ||
                (SKUList[a].soldCount == SKUList[b].soldCount &&
                 strcmp(SKUList[a].name, SKUList[b].name) > 0)) {
                int temp = idx[j];
                idx[j] = idx[j + 1];
                idx[j + 1] = temp;
            }
        }
    }

    printf("\nTop %d Bestsellers:\n", K);
    for (int i = 0; i < K; i++) {
        int k = idx[i];
        printf("   [%d] %s | Sold: %d | Price: ₹%.2f | Stock: %d\n",
               SKUList[k].skuId, SKUList[k].name, SKUList[k].soldCount,
               SKUList[k].price, SKUList[k].stock);
    }
}

void ABC_Analysis_SoldCount() {
    if (skuCount == 0) {
        printf("No SKUs for analysis.\n");
        return;
    }

    int idx[MAX_SKU];
    int totalSold = 0;
    for (int i = 0; i < skuCount; i++) {
        idx[i] = i;
        totalSold += SKUList[i].soldCount;
    }

    if (totalSold == 0) {
        printf("No sales recorded yet.\n");
        return;
    }

    for (int i = 0; i < skuCount - 1; i++) {
        for (int j = 0; j < skuCount - i - 1; j++) {
            int a = idx[j], b = idx[j + 1];
            if (SKUList[a].soldCount < SKUList[b].soldCount) {
                int temp = idx[j];
                idx[j] = idx[j + 1];
                idx[j + 1] = temp;
            }
        }
    }

    printf("\nABC Analysis (by sold count):\n");

    float cumulative = 0.0;
    float limitA = 0.8 * totalSold;
    float limitB = 0.95 * totalSold;
    float catA = 0, catB = 0, catC = 0;

    for (int i = 0; i < skuCount; i++) {
        int k = idx[i];
        cumulative += SKUList[k].soldCount;

        char category;
        if (cumulative <= limitA)
            category = 'A';
        else if (cumulative <= limitB)
            category = 'B';
        else
            category = 'C';

        printf("   %s | Sold: %d | Category: %c\n",
               SKUList[k].name, SKUList[k].soldCount, category);

        if (category == 'A') catA += SKUList[k].soldCount;
        else if (category == 'B') catB += SKUList[k].soldCount;
        else catC += SKUList[k].soldCount;
    }

    printf("\nCategory Totals:\n");
    printf("   A: %.0f units (≈80%%)\n", catA);
    printf("   B: %.0f units (≈15%%)\n", catB);
    printf("   C: %.0f units (≈5%%)\n", catC);
}

void storeData() {
    FILE *TextFile = fopen("SKUMemoryDump.txt", "w"); 
    if (TextFile == NULL) {
        printf("Error creating SKU memory dump file.\n");
        return;
    }

    fprintf(TextFile, "========== SKU MEMORY DUMP ==========\n");
    fprintf(TextFile, "Total SKUs stored: %d\n\n", skuCount);

    if (skuCount == 0)
        fprintf(TextFile, "No SKU records found.\n\n");
    else {
        for (int i = 0; i < skuCount; i++) {
            fprintf(TextFile, "Index: %d\n", i);
            fprintf(TextFile, "SKU ID: %d\n", SKUList[i].skuId);
            fprintf(TextFile, "Name: %s\n", SKUList[i].name);
            fprintf(TextFile, "Category: %s\n", SKUList[i].category);
            fprintf(TextFile, "Price: %.2f\n", SKUList[i].price);
            fprintf(TextFile, "Stock: %d\n", SKUList[i].stock);
            fprintf(TextFile, "Sold Count: %d\n", SKUList[i].soldCount);
            fprintf(TextFile, "---------------------------------\n");
        }
    }
    fclose(TextFile);

    FILE *TextFile1 = fopen("OrderMemoryDump.txt", "w");  
    if (TextFile1 == NULL) {
        printf("Error creating order memory dump file.\n");
        return;
    }

    fprintf(TextFile1, "\n========== ORDER MEMORY DUMP ==========\n\n");
    fprintf(TextFile1, "Total Orders stored: %d\n\n", orderCount);

    if (orderCount == 0)
        fprintf(TextFile1, "No Order records found.\n");
    else {
        for (int i = 0; i < orderCount; i++) {
            fprintf(TextFile1, "Index: %d\n", i);
            fprintf(TextFile1, "Order ID: %d\n", OrderList[i].orderId);
            fprintf(TextFile1, "Order Time: %lld\n", OrderList[i].orderTime);
            fprintf(TextFile1, "Status: %d\n", OrderList[i].status);
            fprintf(TextFile1, "Delivery Time: %lld\n", OrderList[i].deliveryTime);
            fprintf(TextFile1, "Item Count: %d\n", OrderList[i].itemCount);
            fprintf(TextFile1, "Subtotal: %.2f\n", OrderList[i].subtotal);
            fprintf(TextFile1, "Items: ");
            for (int j = 0; j < OrderList[i].itemCount; j++) {
                fprintf(TextFile1, "[ID=%d Qty=%d] ", OrderList[i].itemIds[j], OrderList[i].itemQty[j]);
            }
            fprintf(TextFile1, "\n---------------------------------\n");
        }
    }

    fclose(TextFile1);
}

int main() {
    int choice;

    while (1) {
        printf("\nBLINKIT-LITE SYSTEM\n");
        printf("1. Add SKU\n");
        printf("2. Update SKU\n");
        printf("3. Delete SKU\n");
        printf("4. Place Order\n");
        printf("5. Deliver Order\n");
        printf("6. Cancel Order\n");
        printf("7. Search SKU by Name\n");
        printf("8. Sort Orders by Time\n");
        printf("9. Top-K Bestsellers\n");
        printf("10. ABC Analysis by Sold Count\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 0) {
            printf("Exiting program.\n");
            break;
        }

        switch (choice) {
            case 1: {
                printf("\n--- Add New SKU ---\n");
                Add_SKU();
                storeData();
                break;
            }

            case 2: {
                int id;
                printf("\nEnter SKU ID to update: ");
                scanf("%d", &id);
                Update_or_Delete_SKU(id, 1);
                storeData();
                break;
            }

            case 3: {
                int id;
                printf("\nEnter SKU ID to delete: ");
                scanf("%d", &id);
                Update_or_Delete_SKU(id, 0);
                storeData();
                break;
            }

            case 4: {
                int itemCount;
                long long int orderTime;
                int itemIds[MAX_ITEMS];
                int itemQty[MAX_ITEMS];

                printf("\n--- Place New Order ---\n");
                printf("Enter number of items: ");
                scanf("%d", &itemCount);

                for (int i = 0; i < itemCount; i++) {
                    printf("Enter item %d ID: ", i + 1);
                    scanf("%d", &itemIds[i]);
                    printf("Enter quantity for item %d: ", i + 1);
                    scanf("%d", &itemQty[i]);
                }

                printf("Enter Order Time (YYYYMMDDHHMM): ");
                scanf("%lld", &orderTime);

                Place_Order(itemIds, itemQty, itemCount, orderTime);
                storeData();
                break;
            }

            case 5: {
                int orderId;
                long long int deliveryTime;
                printf("\n--- Deliver Order ---\n");
                printf("Enter Order ID: ");
                scanf("%d", &orderId);
                printf("Enter Delivery Time (YYYYMMDDHHMM): ");
                scanf("%lld", &deliveryTime);
                Deliver_Order(orderId, deliveryTime);
                storeData();
                break;
            }

            case 6: {
                int orderId;
                printf("\n--- Cancel Order ---\n");
                printf("Enter Order ID: ");
                scanf("%d", &orderId);
                Cancel_Order(orderId);
                storeData();
                break;
            }

            case 7: {
                char query[50];
                printf("\n--- Search SKU ---\n");
                printf("Enter part of the name to search: ");
                scanf("%s", query);
                Search_SKU_ByName_Substring(query);
                break;
            }

            case 8: {
                printf("\n--- Sort Orders by Time ---\n");
                Sort_Orders_ByTime();
                break;
            }

            case 9: {
                int K;
                printf("\n--- Top-K Bestsellers ---\n");
                printf("Enter K value: ");
                scanf("%d", &K);
                TopK_Bestsellers(K);
                break;
            }

            case 10: {
                printf("\n--- ABC Analysis ---\n");
                ABC_Analysis_SoldCount();
                break;
            }

            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}




