package woo;

import java.io.Serializable;

public class Normal extends Status implements Serializable {

    public Normal(Client client) {
        super(client, 0);
    }
    
    public Normal(Client client, double points) {
        super(client, points);
    }

    public double getDiscount(int currentDate, int limitDate, int period) {
        if(limitDate - currentDate >= period)
            return 0.9;
        if(0 < currentDate - limitDate && currentDate - limitDate <= period)
            return (currentDate - limitDate) * 0.05 + 1;
        if(currentDate - limitDate > period)
            return (currentDate - limitDate) * 0.10 + 1;
        else
            return 1;
    }

    public void pay(double value, int delay) {
        if(delay <= 0) {
            addPoints(value*10);
            if(getPoints()>25000)
                getClient().setStatus(new Elite(getClient(), getPoints()));
            else if(getPoints()>2000)
                getClient().setStatus(new Selection(getClient(), getPoints()));
        }
    }

    public String toString() {
        return "NORMAL";
    }
}