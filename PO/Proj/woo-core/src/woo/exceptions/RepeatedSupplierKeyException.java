package woo.exceptions;

/** Exception thrown when trying to create a supplier with a repeated key */
public class RepeatedSupplierKeyException extends Exception {

  /** Supplier key. */
  private String _key;

  /**
   * @param key
   */
  public RepeatedSupplierKeyException(String key) {
    _key = key;
  }

  /** @return key */
  public String getKey() {
    return _key;
  }

}
