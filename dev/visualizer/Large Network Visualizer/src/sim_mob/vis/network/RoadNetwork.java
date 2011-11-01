package sim_mob.vis.network;

import java.io.*;
import java.util.*;
import java.util.regex.*;

import sim_mob.vis.network.basic.DPoint;
import sim_mob.vis.util.Utility;

/**
 * The RoadNetwork is the top-level object containing all relevant details about our 
 * static RoadNetwork.
 */
public class RoadNetwork {
	private DPoint cornerTL;
	private DPoint cornerLR;
	
	private Hashtable<Integer, Node> nodes;
	private Hashtable<Integer, Link> links;
	private Hashtable<Integer, Segment> segments;
	private Hashtable<Integer,Hashtable<Integer,Lane>> lanes;
	private Hashtable<Integer, Crossing> crossings;
	
	//Testing on intersections
	private ArrayList<Intersection> intersections;
	private Intersection tempIntersection;
	
	
	public DPoint getTopLeft() { return cornerTL; }
	public DPoint getLowerRight() { return cornerLR; }
	public Hashtable<Integer, Node> getNodes() { return nodes; }
	public Hashtable<Integer, Link> getLinks() { return links; }
	public Hashtable<Integer, Segment> getSegments() { return segments; }
	public Hashtable<Integer,Hashtable<Integer,Lane>> getLanes(){ return lanes; }
	public Hashtable<Integer, Crossing> getCrossings() { return crossings; }
	
	/**
	 * Load the network from a filestream.
	 */
	
	public RoadNetwork(BufferedReader inFile) throws IOException {
		nodes = new Hashtable<Integer, Node>();
		links = new Hashtable<Integer, Link>();
		segments = new Hashtable<Integer, Segment>();
		lanes = new Hashtable<Integer,Hashtable<Integer,Lane>>();
		crossings = new Hashtable<Integer, Crossing>();
		
		//Also track min/max x/y pos
		double[] xBounds = new double[]{Double.MAX_VALUE, Double.MIN_VALUE};
		double[] yBounds = new double[]{Double.MAX_VALUE, Double.MIN_VALUE};
		
		//Read
		String line;
		while ((line=inFile.readLine())!=null) {
			//Comment?
			line = line.trim();
			if (line.isEmpty() || !line.startsWith("(") || !line.endsWith(")")) {
				continue;
			}
			
			//Parse basic
		    Matcher m = Utility.LOG_LHS_REGEX.matcher(line);
		    if (!m.matches()) {
		      throw new IOException("Invalid line: " + line);
		    }
		    if (m.groupCount()!=4) {
		      throw new IOException("Unexpected group count (" + m.groupCount() + ") for: " + line);
		    }

		    //Known fields: type, id, rhs
		    String type = m.group(1);
		    int frameID = Integer.parseInt(m.group(2));
		    int objID = Utility.ParseIntOptionalHex(m.group(3));
		    String rhs = m.group(4);
		    
		    //Pass this off to a different function based on the type
		    try {
		    	dispatchConstructionRequest(type, frameID, objID, rhs, xBounds, yBounds);
		    } catch (IOException ex) {
		    	throw new IOException(ex.getMessage() + "\n...on line: " + line);
		    }
		}
		
		//Save bounds
		cornerTL = new DPoint(xBounds[0], yBounds[0]);
		cornerLR = new DPoint(xBounds[1], yBounds[1]);
		
	}
			
	private void dispatchConstructionRequest(String objType, int frameID, int objID, String rhs, double[] xBounds, double[] yBounds) throws IOException {
		//Nodes are displayed the same
		if (objType.equals("multi-node") || objType.equals("uni-node")) {
			parseNode(frameID, objID, rhs, objType.equals("uni-node"), xBounds, yBounds);
		} else if (objType.equals("link")) {
			parseLink(frameID, objID, rhs);
		} else if (objType.equals("road-segment")) {
			parseSegment(frameID, objID, rhs);
		} else if (objType.equals("lane")){
			parseLane(frameID, objID, rhs);
		} else if(objType.equals("crossing")){
			parseCrossing(frameID, objID, rhs);
		}
	}
		
	private void parseLink(int frameID, int objID, String rhs) throws IOException {
	    //Check frameID
	    if (frameID!=0) {
	    	throw new IOException("Unexpected frame ID, should be zero");
	    }
	    
	    //Check and parse properties.
	    Hashtable<String, String> props = Utility.ParseLogRHS(rhs, new String[]{"road-name", "start-node", "end-node", "fwd-path", "rev-path"});
	    
	    //Now save the relevant information
	    String name = props.get("road-name");
	    int startNodeKEY = Utility.ParseIntOptionalHex(props.get("start-node"));
	    int endNodeKEY = Utility.ParseIntOptionalHex(props.get("end-node"));
	    Node startNode = nodes.get(startNodeKEY);
	    Node endNode = nodes.get(endNodeKEY);
	    
	    //Ensure nodes exist
	    if (startNode==null) {
	    	throw new IOException("Unknown node id: " + Integer.toHexString(startNodeKEY));
	    }
	    if (endNode==null) {
	    	throw new IOException("Unknown node id: " + Integer.toHexString(endNodeKEY));
	    }
	    
	    //Create a new Link, save it
	    links.put(objID, new Link(name, startNode, endNode));
	}
	
	private void parseLane(int frameID, int objID, String rhs) throws IOException {
	 
		//Check frameID
	    if (frameID!=0) {
	    	throw new IOException("Unexpected frame ID, should be zero");
	    }
	    
	    //Check and parse properties. for lanes, it checks only parent-segment only as the number of lanes is not fixed
	    Hashtable<String, String> props = Utility.ParseLogRHS(rhs, new String[]{"parent-segment"});
	    
	    Enumeration<String> keys = props.keys();
	   
	    Hashtable<Integer,Lane> tempLaneTable = new Hashtable<Integer,Lane>();
	    
	    int sideWalkLane = -1;
	    int parentKEY = -1;
	    while(keys.hasMoreElements()){
		    
	    	String key = keys.nextElement().toString();

	    	//Get Segment
	    	if(key.contains("parent-segment")){
	    		
	    		parentKEY = Utility.ParseIntOptionalHex(props.get("parent-segment"));
	    	    
	    		//Ensure Segment exist
	    		if (parentKEY == -1) {
	    	    	throw new IOException("Unknown Segment id: " + Integer.toHexString(parentKEY));
	    	    }
	    		
	    		continue;
	    	}
	    	
	    	//Check whether the lane is a sidewalk
	    	Matcher m = Utility.NUM_REGEX.matcher(key);
	    	Integer laneNumber = null;
	    	while(m.find()){
	    		laneNumber = Integer.parseInt(m.group());
	    		
	    	}
	    	//Extract node information
	    	if(key.contains("sidewalk")){
	    		//keep track the side walk lane number
	    		sideWalkLane = laneNumber;
	    	}else{
	    		
	    		ArrayList<Integer> pos = new ArrayList<Integer>();
	    		pos = Utility.ParseLaneNodePos(props.get(key));
	    		Node startNode = new Node(pos.get(0), pos.get(1), false);
	    		Node endNode = new Node(pos.get(2), pos.get(3), false);

	    		tempLaneTable.put(laneNumber, new Lane(startNode,endNode,false,laneNumber,parentKEY));

	    	}
	   
	    } 
	    
	    //Find the sidewalk lane and mark it 
	    if(sideWalkLane!=-1){
	    	tempLaneTable.get(sideWalkLane).setSideWalk(true);
	    	tempLaneTable.get((sideWalkLane+1)).setSideWalk(true);
	    }
	    
	    //Create a new Lane, save it
	    lanes.put(objID, tempLaneTable);
	    
	}
	
	private void parseSegment(int frameID, int objID, String rhs) throws IOException {
	    //Check frameID
	    if (frameID!=0) {
	    	throw new IOException("Unexpected frame ID, should be zero");
	    }
	    
	    //Check and parse properties.
	    Hashtable<String, String> props = Utility.ParseLogRHS(rhs, new String[]{"parent-link", "max-speed", "lanes", "from-node", "to-node"});
	    
	    //Now save the relevant information
	    int parentKEY = Utility.ParseIntOptionalHex(props.get("parent-link"));
	    Link parent = links.get(parentKEY);
	    int fromNodeKEY = Utility.ParseIntOptionalHex(props.get("from-node"));
	    int toNodeKEY = Utility.ParseIntOptionalHex(props.get("to-node"));
	    Node fromNode = nodes.get(fromNodeKEY);
	    Node toNode = nodes.get(toNodeKEY);
	    
	    //Ensure nodes exist
	    if (parent==null) {
	    	throw new IOException("Unknown Link id: " + Integer.toHexString(parentKEY));
	    }
	    if (fromNode==null) {
	    	throw new IOException("Unknown node id: " + Integer.toHexString(fromNodeKEY));
	    }
	    if (toNode==null) {
	    	throw new IOException("Unknown node id: " + Integer.toHexString(toNodeKEY));
	    }
	    
	    //Create a new Link, save it
	    segments.put(objID, new Segment(parent, fromNode, toNode));

	    

	    //Gathering Intersection Information
/*	    int tempKEY= Utility.ParseIntOptionalHex("0x95b3fd0");

	    if(fromNodeKEY == tempKEY || toNodeKEY == tempKEY){
	    	System.out.println("objID: "+ objID);
	    	tempIntersection.intersectionSegmentsID.add(objID);
	    }*/
	    
	}
	
	private void parseNode(int frameID, int objID, String rhs, boolean isUni, double[] xBounds, double[] yBounds) throws IOException {
	    //Check frameID
	    if (frameID!=0) {
	    	throw new IOException("Unexpected frame ID, should be zero");
	    }
	    
	    //Check and parse properties.
	    Hashtable<String, String> props = Utility.ParseLogRHS(rhs, new String[]{"xPos", "yPos"});
	    
	    //Now save the position information
	    double x = Double.parseDouble(props.get("xPos"));
	    double y = Double.parseDouble(props.get("yPos"));
	    
	    Utility.CheckBounds(xBounds, x);
	    Utility.CheckBounds(yBounds, y);
	    
	    nodes.put(objID, new Node(x, y, isUni));
	}
		
	private void parseCrossing(int frameID, int objID, String rhs) throws IOException {
	    //Check frameID
	    if (frameID!=0) {
	    	throw new IOException("Unexpected frame ID, should be zero");
	    }
	    
	    //Check and parse properties.
	    Hashtable<String, String> props = Utility.ParseLogRHS(rhs, new String[]{"near-1", "near-2", "far-1", "far-2"});
	    
	    //Now save the relevant information
	    Node nearOneNode = Utility.ParseCrossingNodePos(props.get("near-1"));
	    Node nearTwoNode = Utility.ParseCrossingNodePos(props.get("near-2"));
	    Node farOneNode = Utility.ParseCrossingNodePos(props.get("far-1"));
	    Node farTwoNode = Utility.ParseCrossingNodePos(props.get("far-2"));
	   
	    
	    
	    //Create a new Crossing, save it
	    crossings.put(objID, new Crossing(nearOneNode,nearTwoNode,farOneNode,farTwoNode,objID));
	}


}






