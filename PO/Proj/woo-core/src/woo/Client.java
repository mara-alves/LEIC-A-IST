package woo;

import java.io.Serializable;
import java.util.ArrayList;

public class Client extends Observer implements Serializable {
  /** Client's key. */
  private String _key;
  /** Client's name */
  private String _name;
  /** Client's address */
  private String _address;
  /** Client status */
  private Status _status = new Normal(this);
  /** Value of client purchases */
  private double _purchases = 0;
  /** Value of payed purchases */
  private double _payed = 0;
  /** Client's sales */
  private ArrayList<Sale> _sales = new ArrayList<Sale>();
  /** Notification formatter */
  private NotificationFormatter _notificationFormatter = new AppNotifications();
  /** Notifications */
  private ArrayList<String> _notifications = new ArrayList<String>();

  /**
   * Constructor for class Client.
   * 
   * @param key
   *          client key
   * @param name
   *          client name
   * @param address
   *          client address
   */
  public Client(String key, String name, String address) {
    _key = key;
    _name = name;
    _address = address;
  }

  /**
   * @return client key
   */
  public String getKey() {
      return _key;
  }

  /**
   * @return list of notifications
   */
  public ArrayList<String> getNotifications() {
    return _notifications;
  }

  /**
   * Returns sales associated with client.
   * 
   * @return list with all sales.
   */
   public ArrayList<Sale> getSales() {
     return _sales;
   }

  /**
   * @return status of client
   */
  public Status getStatus() {
    return _status;
  }

  /**
   * @param status
   *        status for client
   */
  public void setStatus(Status status) {
    _status = status;
  }

  /**
   * @param currentDate
   *        today's date
   * @param limitDate
   *        payment deadline
   * @param period 
   *        time period considered to calculate discount/penalty
   * @return discount/penalty to be applied
   */
  public double getDiscount(int currentDate, int limitDate, int period) {
    return _status.getDiscount(currentDate, limitDate, period);
  }

  /** 
   * @return string of client
   */
  public String toString() {
    return _key+"|"+_name+"|"+_address+"|"+_status.toString()+"|"
           +(int)_purchases+"|"+(int)_payed;
  }

  /**
   * Registers a sale associated with client.
   * 
   * @param sale
   *          sale
   */
  public void registerSale(Sale sale) {
    _sales.add(sale);
    _purchases += sale.getBaseValue();
  }

  /**
   * Registers a client notification.
   *
   * @param notification
   *        notification to be registered
   */
  public void registerNotification(String notification) {
    _notifications.add(notification);
  }

  /**
   * Toggles notifications for a certain product.
   *
   * @param p
   *        desired product to receive notifications about
   * @return new state of notifications (true if on)
   */
  public boolean toggleNotifications(Product p) {
    if (p.getObservers().containsValue(this)) {
      p.removeObserver(_key);
      return false;
    }
    else {
      p.registerObserver(_key, this);
      return true;
    }
  }

  public void updatedPrice(String productKey, int price) {
    _notificationFormatter.notifyPrice(this, productKey, price);
  }

  public void updatedStock(String productKey, int stock) {
    _notificationFormatter.notifyStock(this, productKey, stock);
  }

  /** 
   * Pays a sale.
   *
   * @param value
   *        cost of sale
   * @param delay
   *        diference between current date and date limit
   */
  public void pay(double value, int delay) {
    _payed += value;
    _status.pay(value, delay);
  }

}