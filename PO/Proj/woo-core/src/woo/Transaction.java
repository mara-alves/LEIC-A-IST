package woo;

import java.io.Serializable;

public abstract class Transaction implements Serializable {
    /** Transaction id */
    private int _key;

    /**
     * Constructor for class Transaction
     */
    public Transaction(int count) {
        _key = count;
    }

    /**
     * @return transaction id
     */
    public int getKey() {
        return _key;
    }

    /** Pay a transaction */
    public abstract void pay(int currentDate);

    /** Value of transaction to calculate balances */
    public abstract double todaysValue(int currentDate);
    public abstract double payedValue();

    /** String of transaction, depends of current date */
    public abstract String showToday(int currentDate);
}