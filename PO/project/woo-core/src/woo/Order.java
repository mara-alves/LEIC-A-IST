package woo;

import java.io.Serializable;
import java.util.Map;
import java.util.LinkedHashMap;

public class Order extends Transaction implements Serializable {
    /* Order total cost */
    private int _totalCost = 0;
    /* Products kept */
    private Map<Product, Integer> _products = new LinkedHashMap<Product, Integer>();
    /* Key of the supplier that received the order */
    private String _supplierKey;
    /* Payment date */
    private int _date;

    /**
     * Constructor for class Order
     *
     * @param product
     * @param quantity
     * @param supplierKey
     * @param date
     */
    public Order(int count, Product product, int quantity,
                 String supplierKey,int date) {
        super(count);
        this.addProduct(product, quantity);
        _supplierKey = supplierKey;
        _date = date;
    }

    /**
     * @return total cost of order
     */
    public int getCost() {
        return _totalCost;
    }

    /**
     * @return supplier key
     */
    public String getSupplierKey() {
        return _supplierKey;
    }

    /**
     * @return total date of payment
     */
    public int getPaymentDate() {
        return _date;
    }

    /** 
    * @return string of order
    */
    public String toString() {
        String res = super.getKey()+"|"+getSupplierKey()+"|"+getCost()
                     +"|"+getPaymentDate();

        for(Map.Entry<Product,Integer> entry : _products.entrySet()) {
            res += "\n" + entry.getKey().getKey() + "|" + entry.getValue();
        }
        return res;
    }

    /**
     * @param currentDate 
     *          date when the value is requested
     * @return value of order today
     *          negative because the company loses money 
     *          when ordering from supplier
     */
    public double todaysValue(int currentDate) {
        return -getCost();
    }

    /**
     * @return the actual payed price by the company
     */
    public double payedValue() {
        return -getCost();
    }

    /**
     * @param date
     *          date when string is requested
     * @return string of order
     */
    public String showToday(int date) {
        return toString();
    }

    /**
     * Adds a product to order
     *
     * @param product
     *          first product
     * @param quantity
     *          amout of product
     */
    public void addProduct(Product product, int quantity) {
        /* if product is already in order add new amount to previous */
        if(_products.get(product) != null) {
            int newQuantity = _products.get(product) + quantity;
            _products.put(product, newQuantity);
        }
        else
            _products.put(product, quantity);

        _totalCost += quantity*product.getPrice();
    }

    /**
     * Reverts stock of products in order,
     * when order is canceled before concluded.
     */
    public void cancel() {
        for(Map.Entry<Product,Integer> entry : _products.entrySet())
            entry.getKey().removeStock(entry.getValue());
    }

    /**
     * Pays an order
     * currently doesn't do anything, because orders are payed
     * when registered
     *
     * @param currentDate
     *          day of payment
     */
    public void pay(int currentDate) { /* do nothing */ }
}