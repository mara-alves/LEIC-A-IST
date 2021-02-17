package woo;

import java.io.Serializable;

public class Sale extends Transaction implements Serializable {
    /* Product sold */
    private Product _product;
    /* Quantity of product */
    private int _quantity;
    /* Date limit to pay */
    private int _dateLimit;
    /* Client */
    private Client _client;
    /* Payed ? */
    private boolean _payed = false;
    /* Payment date */
    private int _paymentDate;
    /* Value payed */
    private double _valuePayed;

    /**
    * Constructor for class Sale.
    * 
    * @param product
    *          product
    * @param quantity
    *          amount of product being sold
    * @param dateLimit
    *          payment deadline
    * @param client
    *          client associated with sale
    */
    public Sale(int count, Product product, int quantity, 
                int dateLimit, Client client) {
        super(count);
        _product = product;
        _quantity = quantity;
        _dateLimit = dateLimit;
        _client = client;
    }

    /**
     * @return key of product
     */
    public String getProductKey() {
        return _product.getKey();
    }

    /**
     * @return quantity of product
     */
    public int getQuantity() {
        return _quantity;
    }

    /**
     * @return date limit of payment
     */
    public int getDateLimit() {
        return _dateLimit;
    }

    /**
     * @return client
     */
    public Client getClient() {
        return _client;
    }

    /**
     * @return base value, no discount or penalties
     */
    public double getBaseValue() {
        return _product.getPrice()*_quantity;
    }

    /**
     * @param currentDate
     *          date when the price is requested
     * @return real price, with penalties or discounts
     */
    public double getRealPrice(int currentDate) {
        return getBaseValue()
               * _client.getDiscount(currentDate, _dateLimit,
                                     _product.getPeriod());
    }

    /**
     * @return date of payment
     */
    public int getPaymentDate() {
        return _paymentDate;
    }

    /**
     * @return true if sale was payed, false if not
     */
    public boolean isPayed() {
        return _payed;
    }

    /**
     * @param date
     *          date when string is requested
     * @return string of sale, depends on the date
     */
    public String showToday(int date) {
        String res = super.getKey()+"|"+getClient().getKey()+"|"
                     +getProductKey()+"|"+getQuantity()+"|"
                     +(int)getBaseValue()+"|";
        if(_payed)
            res += (int)_valuePayed+"|"+getDateLimit()+"|"+getPaymentDate();
        else
            res += (int)getRealPrice(date)+"|"+getDateLimit();
        return res;
    }

    /**
     * @param currentDate 
     *          date when the value is requested
     * @return value of sale on that day
     *          positive for the company
     */
    public double todaysValue(int currentDate) {
        if(!_payed)
            return getRealPrice(currentDate);
        else
            return _valuePayed;
    }

    /**
     * @return the actual payed price by the client
     *          positive for the company
     */
    public double payedValue() {
        if(!_payed)
            return 0;
        else
            return _valuePayed;
    }

    /**
     * Pays a sale
     *
     * @param currentDate
     *          day of payment
     */
    public void pay(int currentDate) {
    if(!_payed) {
      _payed = true;
      _paymentDate = currentDate;
      _valuePayed = getRealPrice(currentDate);
      _client.pay(getRealPrice(currentDate), currentDate-getDateLimit());
    }
  }

}