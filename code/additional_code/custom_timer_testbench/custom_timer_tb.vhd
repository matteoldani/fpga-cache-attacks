----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/14/2022 08:52:02 PM
-- Design Name: 
-- Module Name: cutum_timer_tb - Behavioral
-- Project Name: 
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

entity cutum_timer_tb is

end cutum_timer_tb;

architecture Behavioral of cutum_timer_tb is

    COMPONENT custum_timer
    PORT(
        Clk             : in std_logic;
        rst             : in std_logic;
        arv             : in std_logic;
        arr             : in std_logic;
        rv              : in std_logic;
        rr              : in std_logic;
        counter_valid   : out std_logic;
        counter         : out std_logic_vector(31 downto 0)
    );
    END COMPONENT;
    
    signal Clk      : std_logic := '0';
    signal rst      : std_logic := '0';
    
    signal arv      : std_logic := '0';
    signal arr      : std_logic := '1';
    signal rr       : std_logic := '0';
    signal rv       : std_logic := '1';
    
    signal counter  : std_logic_vector(31 downto 0) := (others => '0');
    signal counter_valid : std_logic := '0';

begin

    Clk <= not Clk after 1 ns;
    rst <= '0', '1' after 1 ns;
    
    uut: custum_timer PORT MAP (
        arv => arv,
        arr => arr,
        rr => rr,
        rv => rv, 
        rst => rst,
        Clk => Clk,
        counter => counter,
        counter_valid => counter_valid
        );
        
    stimulus:
    process begin
        wait until(rst = '1');
        
        wait for 4 ns;
        arv <= '1';
        
        wait for 2 ns;
        arr <= '0';
        arv <= '0';
        
        wait for 2ns;
        arr <= '1';
        
        wait for 20 ns;
        rr <= '1';
        wait for 2 ns;
        rr <= '0';
        
        

    end process stimulus;

end Behavioral;
