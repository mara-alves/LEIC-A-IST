package woo.exceptions;

/** 
 * Exception thrown when trying to register and order
 * but supplier isn't active for transactions
 */
public class TransactionsOffException extends Exception {

  /** Supplier key. */
  private String _key;

  /**
   * @param key
   */
  public TransactionsOffException(String key) {
    _key = key;
  }

  /** @return key */
  public String getKey() {
    return _key;
  }

}
