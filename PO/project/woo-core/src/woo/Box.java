package woo;

import java.io.Serializable;
import java.util.List;

public class Box extends Product implements Serializable {
  
  /* Types of service */
  public enum ServiceType {
    NORMAL, AIR, EXPRESS, PERSONAL
  }

  /* Type of service */
  ServiceType _serviceType;
  /* Products kept */
  private List<Product> _products;

  /**
   * Constructor for class Box.
   * 
   * @param key
   *          product key
   * @param serviceType
   *          box service type
   * @param supplier
   *          product's supplier
   * @param price
   *          price of product
   * @param criticalLevel
   *          critical value of product
   * @param stock
   *          current stock of book
   */
  public Box(String key, String serviceType, Supplier supplier,
             int price, int criticalLevel, int stock) {
    super(5, key, supplier, price, criticalLevel, stock);
     _serviceType = ServiceType.valueOf(serviceType);
  }

  public Box(int period, String key, String serviceType, Supplier supplier,
             int price, int criticalLevel, int stock) {
    super(period, key, supplier, price, criticalLevel, stock);
     _serviceType = ServiceType.valueOf(serviceType);
  }

  /**
   * @return service type as string
   */
  public String getServiceType() {
    return _serviceType.name();
  }

  /** 
   * @return string of box
   */
  public String toString() {
    return "BOX"+"|"+super.getKey()+"|"+super.getSupplierKey()+"|"
            +super.getPrice()+"|"+ super.getCriticalLevel()+"|"
            +super.getStock()+"|"+getServiceType();
  }

}