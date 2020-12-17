package woo.exceptions;

/** Exception thrown when trying to buy too many of a vertain product */
public class NotEnoughProductException extends Exception {

  /** Product key. */
  private String _key;

  /** Available amout of product */
  private int _available;

  /**
   * @param key
   */
  public NotEnoughProductException(String key, int available) {
    _key = key;
    _available = available;
  }

  /** @return key */
  public String getKey() {
    return _key;
  }

  /** @return available */
  public int getAvailable() {
    return _available;
  }
}