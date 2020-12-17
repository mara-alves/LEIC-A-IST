package woo;

import java.io.Serializable;

public abstract class Status implements Serializable {
    private Client _client;
    private double _points;

    public Status(Client client, double points) {
        _client = client;
        _points = points;
    }

    public Client getClient() {
        return _client;
    }

    public double getPoints() {
        return _points;
    }

    public void addPoints(double points) {
        _points += points;
    }

    public void losePoints(double points) {
        _points -= points;;
    }

    public abstract double getDiscount(int currentDate, int limitDate, 
                                       int period);
    public abstract void pay(double value, int delay);
    public abstract String toString();
}