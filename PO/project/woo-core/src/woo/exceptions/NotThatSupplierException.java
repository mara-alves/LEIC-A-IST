package woo.exceptions;

/** Exception for reporting wrong supplier/product associations. */
public class NotThatSupplierException extends Exception {

  /** Supplier key. */
  private String _skey;

  /** Product key. */
  private String _pkey;

  /**
   * @param skey
   * @param pkey
   */
  public NotThatSupplierException(String skey, String pkey) {
    _skey = skey;
    _pkey = pkey;
  }

  /** @return supplier key */
  public String getSupplierKey() {
    return _skey;
  }

  /** @return product key */
  public String getProductKey() {
    return _pkey;
  }

}
