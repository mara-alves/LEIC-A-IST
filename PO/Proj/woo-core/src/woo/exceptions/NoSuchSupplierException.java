package woo.exceptions;

/** Exception thrown when supplier requested by key doesn't exist */
public class NoSuchSupplierException extends Exception {

  /** Supplier key. */
  private String _key;

  /**
   * @param key
   */
  public NoSuchSupplierException(String key) {
    _key = key;
  }

  /** @return key */
  public String getKey() {
    return _key;
  }

}
