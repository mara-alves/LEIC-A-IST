package woo;

import java.io.Serializable;
import java.util.ArrayList;

public class Supplier implements Serializable {
  /** Supplier's key. */
  private String _key;
  /** Supplier's name */
  private String _name;
  /** Supplier's address */
  private String _address;
  /** Is the supplier active for transactions? */
  private boolean _activeTransactions = true;
  /* Supplier's orders */
  private ArrayList<Order> _orders = new ArrayList<Order>();

  /**
   * Constructor for class Supplier.
   * 
   * @param key
   *          supplier key
   * @param name
   *          supplier name
   * @param address
   *          supplier address
   */
  public Supplier(String key, String name, String address) {
    _key = key;
    _name = name;
    _address = address;
  }

  /**
   * @return active transactions?
   */
  public boolean isActive() {
    return _activeTransactions;
  }

  /**
   * @return the supplier key
   */
  public String getKey() {
    return _key;
  }

  /**
   * Returns orders associated with supplier.
   * 
   * @return list with all orders.
   */
   public ArrayList<Order> getOrders() {
     return _orders;
   }

  /** 
   * @return string of supplier
   */
  public String toString() {
    return _key+"|"+_name+"|"+_address+"|"
           +(_activeTransactions ? "SIM" : "NÃO");
  }

  /**
   * Turn on/off transactions
   */
  public boolean toggleTransactions() {
    _activeTransactions = !_activeTransactions;
    return _activeTransactions;
  }

  /**
   * Registers an order associated with supplier.
   *
   * @param Order
   *          order to register
   */
  public void registerOrder(Order order) {
    _orders.add(order);
  }

  /**
   * Removes last order.
   */
   public void cancelLastOrder() {
     _orders.get(_orders.size()-1).cancel();
     _orders.remove(_orders.size()-1);
   }

  /**
   * Adds a product to the last order
   */
   public void addToLastOrder(Product product, int quantity) {
     _orders.get(_orders.size()-1).addProduct(product, quantity);
   }

}