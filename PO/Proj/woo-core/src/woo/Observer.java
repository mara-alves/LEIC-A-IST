package woo;

import java.io.Serializable;

public abstract class Observer implements Serializable {
    public abstract void updatedPrice(String productKey, int stock);
    public abstract void updatedStock(String productKey, int stock);
}