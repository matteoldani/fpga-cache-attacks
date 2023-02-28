----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/13/2022 10:11:01 AM
-- Design Name: 
-- Module Name: custum_timer - rtl
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
use ieee.numeric_std.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity custum_timer is
    port(
        Clk             : in std_logic;
        rst             : in std_logic;
        arv             : in std_logic;
        arr             : in std_logic;
        rv              : in std_logic;
        rr              : in std_logic;
        counter_valid   : out std_logic;
        counter         : out std_logic_vector(31 downto 0)
    );
end custum_timer;

architecture rtl of custum_timer is

    signal ticks                : unsigned(31 downto 0) := (others => '0');
    signal counter_valid_i      : std_logic := '0';
    signal countable            : std_logic := '0';

begin

    process(Clk, rst, arr, arv, rr, rv) is 
    begin
        if rst = '0' then
            counter_valid_i <= '0';
            ticks <= (others => '0');
        elsif rising_edge(Clk) then
            counter_valid <= counter_valid_i;
            counter <= std_logic_vector(ticks); 
            if arr = '1' and arv = '1' then
                ticks <= (others => '0'); 
                counter_valid_i     <= '0';
                countable           <= '1';
            elsif rr = '1'and rv = '1' then
                counter_valid_i <= '1';
                countable <= '0';
            end if; 
            
            if countable = '1' then
                ticks <= ticks + 1;
            end if;   
        end if; 
    end process;

end rtl;
