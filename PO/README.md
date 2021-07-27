# PO Project 2020-21
This is a project I made for the course called *Programação com Objetos*, with Margarida Rodrigues. Consists on a management application for a distribution company.
Our final grade was `17/20`.

The UML does NOT represent the final code: we did the UML first. And it has quite some mistakes. Sorry about that.

## How To Compile
- `export CLASSPATH=<folder>/po-uuilib/po-uuilib.jar:<folder>/woo-app/woo-app.jar:<folder>/woo-core/woo-core.jar`
- inside woo-core, `make`
- inside woo-app, modify the Makefile so that *JAVADIR* is your po-uuilib directory (`<folder>/po-uuilib`)
- then, still inside woo-app, `make`

## How To Run
- `java woo.app.App`

OR
- `java -Dimport=file.txt woo.app.App`

## Import File
Please keep in mind that this project doesn't contemplate error verifications in the import file. Here is how the text in the import file must be:

#### To register suppliers
`SUPPLIER|id|name|address`
#### To register a client
`CLIENT|id|name|address`
#### To register a box
`BOX|id|service-type|id-supplier|price|critical-value|stock`
#### To register a container
`CONTAINER|id|service-type|service-level|id-supplier|price|critical-value|stock`
#### To register a book
`BOOK|id|title|author|isbn|id-supplier|price|critical-value|stock`

In the import file, all suppliers and clients must be registered before products!