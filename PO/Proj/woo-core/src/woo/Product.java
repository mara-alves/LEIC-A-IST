package woo;

import java.io.Serializable;

public class Product extends Subject implements Serializable {
  /** Product key */
  private String _key;
  /** Supplier associated to the product */
  private Supplier _supplier;
  /** Price of product */
  private int _price;
  /** Critical value of product */
  private int _criticalLevel;
  /** Quantity in stock */
  private int _stock = 0;
  /** Number of days of period */
  private int _period;

  /**
   * Constructor for class Product.
   * 
   * @param period
   *          period considered to calculate discounts and penalties
   * @param key
   *          product key
   * @param supplier
   *          product's supplier
   * @param price
   *          price of product
   * @param criticalLevel
   *          critical value of product
   * @param stock
   *          current stock of product
   */
  public Product(int period, String key, Supplier supplier,
                 int price, int criticalLevel, int stock) {
    _period = period;
    _key = key;
    _supplier = supplier;
    _price = price;
    _criticalLevel = criticalLevel;
    _stock = stock;
  }

  /**
   * @return the product key
   */
  public String getKey() {
    return _key;
  }

  /**
   * @return the key of the supplier associated with this product
   */
  public String getSupplierKey() {
    return _supplier.getKey();
  }

  /**
   * @return the product price
   */
  public int getPrice() {
    return _price;
  }

  /**
   * @return the product critical Level
   */
  public int getCriticalLevel() {
    return _criticalLevel;
  }

  /**
   * @return the amount of product in stock
   */
  public int getStock() {
    return _stock;
  }

  /**
   * @return number of days to pay
   */
  public int getPeriod() {
    return _period;
  }

  /**
   * Change price of product.
   *
   * @param price
   *          new price
   */
  public void setPrice(int price) {
    if(price < _price) {
      _price = price;
      priceDrop();
    }
    else
      _price = price;
  }

  /**
   * Add stock.
   *
   * @param amount
   *          amount of product to be added
   */
  public void addStock(int amount) {
    if(_stock == 0 && amount > 0) {
      _stock += amount;
      addedStock();
    }
    else
      _stock += amount;
  }

  /**
   * Remove stock.
   *
   * @param amount
   *          amount of product to be removed
   */
  public void removeStock(int amount) {
    _stock -= amount;
  }


  public void priceDrop() {
    notifyObserversPrice();
  }

  public void addedStock() {
    notifyObserversStock();
  }

  /**
   * Notify Observers of price drop.
   */
  public void notifyObserversPrice() {
    for (Observer observer: getObservers().values())
        observer.updatedPrice(getKey(), getPrice());
  }

  /**
   * Notify Observers of added stock.
   */
  public void notifyObserversStock() {
    for (Observer observer: getObservers().values())
        observer.updatedStock(getKey(), getStock());
  }

}