# Web-Browser-and-Search-Engine
Designing and implementing a mini search engine capable of handling a collection of web pages hosted on a local server. The project is divided into four main parts, each focusing on a critical component of search engine functionality. The overarching goal is to create a web tool with the following capabilities:

- Displaying Web Pages: Given a URL, the tool can display the corresponding web page.
- Connectivity Information: The tool can display and answer questions about the connectivity of web pages.
- Search Functionality: Users can input queries to find relevant web pages, which will be displayed in order of best to worst match. The tool can automatically display the best matching result.
- GUI Enhancements: A graphical user interface allows for interactive use of the tool, including displaying search results and navigating web pages.

- Analyzing the Content of Web Pages:
 To analyze web pages based on their content, enabling the rating of relevance to user queries.
 Count the frequency of words on each web page, which correlates to the content's relevance.

- Processing User Queries:
 To determine the relevance of a web page to a given query.
 Use word frequency counts to rate relevance, initially using a simplistic approach where relevance is the sum of the frequency counts of query words.

- Adding a Cache and GUI Front-End:
To improve response times and provide a user-friendly interface.
Cache query results to reuse previously calculated relevancies and create a GUI for easy interaction.

- Adding a Hyperlink Graph:
 To visualize and explore web connectivity.
 Build a graph of URL links by parsing web pages and their hyperlinks, allowing users to explore reachable URLs and the shortest paths between them.
