----------------------------------------------------------------------------------
-- Engineer: Matteo Oldani
-- 
-- Create Date: 10/24/2022 02:34:06 PM
-- Design Name: 
-- Module Name: mux - Behavioral
-- Project Name: Semester project Fall 2022
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity mux is
    Port (
        s: in std_logic;
        eviction_signal: in std_logic_vector(3 downto 0);
        axi_signal: in std_logic_vector(3 downto 0);
        out_signal: out std_logic_vector(3 downto 0)
    );
end mux;

architecture rtl of mux is

begin
    out_signal <= axi_signal when (s = '0') else eviction_signal;
end rtl;
