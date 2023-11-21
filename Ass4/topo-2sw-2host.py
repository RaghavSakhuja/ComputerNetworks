"""Custom topology example

Two directly connected switches plus a host for each switch:

   host --- switch --- switch --- host

Adding the 'topos' dict with a key/value pair to generate our newly defined
topology enables one to pass in '--topo=mytopo' from the command line.
"""

from mininet.topo import Topo

class MyTopo( Topo ):
    "Simple topology example."

    def build( self ):
        "Create custom topo."
        s1 = self.addSwitch('s1')
        s2 = self.addSwitch('s2')
        s3 = self.addSwitch('s3')

        # Create hosts
        h1 = self.addHost('h1')
        h2 = self.addHost('h2')
        h3 = self.addHost('h3')
        h4 = self.addHost('h4')
        h5 = self.addHost('h5')
        h6 = self.addHost('h6')
        h7 = self.addHost('h7')
        h8 = self.addHost('h8')
        

        # Connect hosts to switches
        self.addLink(h1, s1)
        self.addLink(h2, s1)
        
        self.addLink(h5, s2)
        self.addLink(h3, s2)
        self.addLink(h4, s2)
        
        self.addLink(h7, s3)
        self.addLink(h8, s3)
        self.addLink(h6, s3)

        # Connect switches to each other
        self.addLink(s1, s2)
        self.addLink(s2, s3)


topos = { 'mytopo': ( lambda: MyTopo() ) }


# sudo mn --custom ~/mininet/custom/topo-2sw-2host.py --topo mytopo --mac --switch ovsk --controller remote
# sudo tc qdisc add dev h1-eth0 root netem delay 10ms rate 1mbit
# sudo ./pox/pox.py forwarding.l2_learning

