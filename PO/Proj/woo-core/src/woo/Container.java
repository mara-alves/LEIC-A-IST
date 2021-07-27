package woo;

import java.io.Serializable;

public class Container extends Box implements Serializable {
  
  /* Possible Service Levels */
  public enum ServiceLevel {
    B4, C4, C5, DL
  }

  /* Service level */
  private ServiceLevel _serviceLevel;

  /**
   * Constructor for class Container.
   * 
   * @param key
   *          product key
   * @param serviceType
   *          container service type
   * @param serviceLevel
   *          container service level
   * @param supplier
   *          product's supplier
   * @param price
   *          price of product
   * @param criticalLevel
   *          critical value of product
   * @param stock
   *          current stock of book
   */
  public Container(String key, String serviceType, String serviceLevel, 
                   Supplier supplier, int price, int criticalLevel, int stock){
    super(8, key, serviceType, supplier, price, criticalLevel, stock);
    _serviceLevel = ServiceLevel.valueOf(serviceLevel);
  }

  /**
   * @return service level as string
   */
  public String getServiceLevel() {
    return _serviceLevel.name();
  }

  /** 
   * @return string of container
   */
  public String toString() {
    return "CONTAINER"+"|"+super.getKey()+"|"+super.getSupplierKey()
           +"|"+super.getPrice()+"|"+ super.getCriticalLevel()+"|"
           +super.getStock()+"|"+super.getServiceType()+"|"+getServiceLevel();
  }

}