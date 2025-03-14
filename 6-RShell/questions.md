1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

_We can read the stream until we hit a delimiter like a null terminator or an EOF signal, which would signify the end of the transmission._

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

_You can handle this by keeping track of the length of the file and add an EOF character at the end. If the length is wrong or the EOF is missed/not added then there is a possibility the functionality would keep reading the stream of bytes until it hits another EOF._

3. Describe the general differences between stateful and stateless protocols.

_In stateful protocols, the server retains a state which has information about client, where following requests depend on the previous one since the server knows the context. Stateless protocols don't keep the information about the client, therefore meaning that each request is independent of any other one._

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

_We would use UDP when the speed is important but the reliability isn't. So for things like games or streams it would be useful when you need information right away but you can miss some packets._

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

_Sockets._