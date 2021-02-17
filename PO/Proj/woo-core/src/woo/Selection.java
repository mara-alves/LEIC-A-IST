package woo;

import java.io.Serializable;

public class Selection extends Status implements Serializable {

    public Selection(Client client, double points) {
        super(client, points);
    }

    public double getDiscount(int currentDate, int limitDate, int period) {
        if(limitDate - currentDate >= period)
            return 0.9;

        if(0 <= limitDate-2 - currentDate 
           && limitDate-2 - currentDate < period)
            return 0.95;

        if(0 < currentDate - limitDate-1 
           && currentDate - limitDate-1 <= period)
            return (currentDate - limitDate) * 0.02 + 1;

        if(currentDate - limitDate > period)
            return (currentDate - limitDate) * 0.05 + 1;

        return 1;
    }

    public void pay(double value, int delay) {
        if(delay > 2)
            getClient().setStatus(new Normal(getClient(), getPoints()*0.1));
        else if(delay <= 0) {
            addPoints(value*10);
            if(getPoints()>25000)
                getClient().setStatus(new Elite(getClient(), getPoints()));
        }
    }

    public String toString() {
        return "SELECTION";
    }
}