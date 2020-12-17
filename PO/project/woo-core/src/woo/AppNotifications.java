package woo;

import java.io.Serializable;

public class AppNotifications implements NotificationFormatter, Serializable {

    public void notifyPrice(Client client, String productKey, int price) {
        String notification = "BARGAIN|" + productKey + "|" + price;
        client.registerNotification(notification);
    }
    public void notifyStock(Client client, String productKey, int stock) {
        String notification = "NEW|" + productKey + "|" + stock;
        client.registerNotification(notification);
    }
}