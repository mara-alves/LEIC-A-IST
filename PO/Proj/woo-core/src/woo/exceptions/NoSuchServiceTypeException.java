package woo.exceptions;

/** Exception thrown when service type doesn't exist */
public class NoSuchServiceTypeException extends Exception {

  /** Service Type */
  private String _serviceType;

  /**
   * @param serviceType
   */
  public NoSuchServiceTypeException(String serviceType) {
    _serviceType = serviceType;
  }

  /** @return service type */
  public String getServiceType() {
    return _serviceType;
  }

}