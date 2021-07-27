package woo;

import java.util.Map;
import java.util.TreeMap;
import java.io.Serializable;

public abstract class Subject implements Serializable {

  /** Observers of product */
  private Map<String, Observer> _observers = new TreeMap<String, Observer>();

  /**
   * @param observers
   *        map of observers to be registered
   */
  public void registerObservers(Map<String, ? extends Observer> observers) {
    _observers.putAll(observers);
  }

  /**
   * @param observer
   *        observer to be registered
   */
  public void registerObserver(String key, Observer observer) {
    _observers.put(key, observer);
  }

  /**
   * @param key
   *        key of observer to be removed
   */
  public void removeObserver(String key) {
      _observers.remove(key);
  }

  /**
   * @return observers
   */
  public Map<String, Observer> getObservers() {
    return _observers;
  }

  /**
   * Notify Observers.
   */
  public abstract void notifyObserversPrice();
  public abstract void notifyObserversStock();

}