# Data queue for working according to the Producer-Consumer scheme.

The main functional requirement is that a Producer can put data (possibly with a wait, if the queue is already full), and a Consumer can take data (possibly with a wait, if there is no data) while maintaining the order.

Levels of development:

1. Just an auxiliary module, built into the project. Accordingly, it is necessary to make a software module, which can then be used in various projects.

   Requirements at this stage:

    * of course, the ability to store data of any type
    * thread safety (well, this is a basic requirement)
    * optional support for operating modes (one Producer - many Producers; one Consumer - many Consumers)
    * optional data prioritization mechanism (priority queue)
    * unit tests and performance tests
1. Everything that is implemented in paragraph 1 plus the ability to save the queue to disk and then load it back.
1. Everything that is implemented in paragraphs 1 and 2, but now as a separate service with the ability to work over the network. Thinking about Kafka at this point is quite natural :).
