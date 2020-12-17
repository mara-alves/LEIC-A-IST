package woo;

public interface NotificationFormatter {
    public void notifyPrice(Client client, String productKey, int price);
    public void notifyStock(Client client, String productKey, int stock);
}