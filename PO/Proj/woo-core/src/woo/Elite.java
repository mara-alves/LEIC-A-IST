package woo;

import java.io.Serializable;

public class Elite extends Status implements Serializable {

    public Elite(Client client, double points) {
        super(client, points);
    }

    public double getDiscount(int currentDate, int limitDate, int period) {
        if(limitDate - currentDate >= period)
            return 0.9;
        if(0 <= limitDate - currentDate && limitDate - currentDate < period)
            return 0.9;
        if(0 < currentDate - limitDate && currentDate - limitDate <= period)
            return 0.95;
        else
            return 1;
    }

    public void pay(double value, int delay) {
        if(delay > 15)
            getClient().setStatus(new Selection(getClient(), getPoints()*0.25));
        else if(delay <= 0)
            addPoints(value*10);
    }

    public String toString() {
        return "ELITE";
    }
}